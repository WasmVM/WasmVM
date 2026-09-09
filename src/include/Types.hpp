#ifndef WASMVM_PP_TYPES_DEF
#define WASMVM_PP_TYPES_DEF

#include <vector>
#include <variant>
#include <optional>
#include <utility>
#include <string>
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>

namespace WasmVM {

using u32_t = uint32_t;
using u64_t = uint64_t;
using i32_t = int32_t;
using i64_t = int64_t;
using f32_t = float;
using f64_t = double;
static_assert(sizeof(f32_t) == sizeof(i32_t));
static_assert(sizeof(f64_t) == sizeof(i64_t));

using u8_t = uint8_t;
using i8_t = int8_t;
using u16_t = uint16_t;
using i16_t = int16_t;

using byte_t = std::byte;
using index_t = u32_t;
using offset_t = u64_t;
using align_t = uint8_t;
constexpr index_t index_npos = (index_t) -1;
using funcref_t = std::optional<index_t>;
using externref_t = void*;

// ── The garbage-collected heap ──────────────────────────────────────────────
// Everything a guest allocates -- a struct, an array, an exception payload --
// is reference counted, which reclaims promptly and predictably but cannot
// reclaim a cycle. A cycle is easy for a guest to build: any mutable
// reference field pointing back at its own object graph makes one, which is
// what a doubly-linked list or a tree with parent pointers is. So a cycle
// collector runs on top of the counts, and nothing is left uncollected.
//
// The collector is Bacon and Rajan's synchronous trial deletion: it works
// from the objects whose count was decremented without reaching zero, so it
// never has to enumerate roots. That matters here because a Value can be
// held by the embedder -- Stack::run() hands a vector of them back -- where
// no tracing collector could see it. The count remains the truth; the
// collector only settles what the count cannot.
struct GcObject {
    virtual ~GcObject() = default;
    // Every GC reference this object holds, for the collector to walk.
    virtual void gc_children(void (*visit)(GcObject*, void*), void* context) const = 0;
    // Let go of every GC reference, so that a cycle can be taken apart while
    // all of its members are still allocated.
    virtual void gc_clear_children() = 0;

    // Collector state. `count` is atomic because two embedder threads may
    // drive two stores at once, which the shared_ptr this replaces allowed.
    std::atomic<std::size_t> gc_count{0};
    enum class GcColour : std::uint8_t { black, gray, white, purple };
    GcColour gc_colour = GcColour::black;
    bool gc_buffered = false;
    // Claimed by the collector as part of a cycle it is taking apart.
    bool gc_doomed = false;
};

void gc_acquire(GcObject* object);
void gc_release(GcObject* object);

/// Collect now. Frees every cycle that has become unreachable, and anything
/// whose count had already reached zero while it waited in the collector's
/// candidate buffer. Returns how many objects it freed in total. An embedder
/// need not call this -- it runs on its own as candidates gather -- but a
/// test or a memory-sensitive caller may want a definite point.
std::size_t gc_collect();
/// How many objects the guest has allocated and not yet had reclaimed. An
/// object whose count has reached zero may still be counted here until the
/// next collection, if it was waiting in the candidate buffer.
std::size_t gc_live_count();

// A counted handle on a GcObject. Null when it holds nothing, and shaped like
// the shared_ptr it replaces so that holding, comparing and dereferencing one
// read the same at every use.
template <typename T>
class GcPtr {
public:
    constexpr GcPtr() noexcept : pointer(nullptr) {}
    constexpr GcPtr(std::nullptr_t) noexcept : pointer(nullptr) {}
    explicit GcPtr(T* raw) : pointer(raw) { if(pointer){ gc_acquire(pointer); } }
    GcPtr(const GcPtr& other) : pointer(other.pointer) { if(pointer){ gc_acquire(pointer); } }
    GcPtr(GcPtr&& other) noexcept : pointer(other.pointer) { other.pointer = nullptr; }
    ~GcPtr(){ if(pointer){ gc_release(pointer); } }

    GcPtr& operator=(const GcPtr& other){
        if(this != &other){
            T* previous = pointer;
            pointer = other.pointer;
            if(pointer){ gc_acquire(pointer); }
            if(previous){ gc_release(previous); }
        }
        return *this;
    }
    GcPtr& operator=(GcPtr&& other) noexcept {
        if(this != &other){
            T* previous = pointer;
            pointer = other.pointer;
            other.pointer = nullptr;
            if(previous){ gc_release(previous); }
        }
        return *this;
    }

    T* operator->() const { return pointer; }
    T& operator*() const { return *pointer; }
    T* get() const { return pointer; }
    explicit operator bool() const { return pointer != nullptr; }

    friend bool operator==(const GcPtr& a, const GcPtr& b){ return a.pointer == b.pointer; }
    friend bool operator==(const GcPtr& a, std::nullptr_t){ return a.pointer == nullptr; }

private:
    T* pointer;
};

// Allocate a GC object and take the first reference to it.
template <typename T, typename... Args>
GcPtr<T> gc_new(Args&&... args){
    return GcPtr<T>(new T(std::forward<Args>(args)...));
}

// Forward declaration for exnref_t
struct WasmException;

struct exnref_t {
    GcPtr<WasmException> value; // null when it holds nothing
    exnref_t() = default;
    explicit exnref_t(GcPtr<WasmException> e) : value(std::move(e)) {}
    bool has_value() const { return value != nullptr; }
};

struct i31ref_t { // stores 31-bit value; nullopt = null i31ref
    std::optional<uint32_t> value;
    constexpr i31ref_t() : value(std::nullopt) {}
    constexpr explicit i31ref_t(uint32_t v) : value(v) {}
    constexpr bool has_value() const { return value.has_value(); }
    constexpr uint32_t operator*() const { return *value; }
};

constexpr u64_t page_size = 65536;

// A struct or array on the GC heap: a reference is null when the handle is.
struct StructObj;
struct ArrayObj;
using structref_t = GcPtr<StructObj>;
using arrayref_t = GcPtr<ArrayObj>;

using Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t,
                           i31ref_t, exnref_t, structref_t, arrayref_t>;

// A value type is a number type or a reference type, and a reference type is
// `(ref null? heaptype)`. The heap type is abstract -- func, extern, any, eq,
// i31, struct, array, exn and the three bottom types -- or concrete, naming a
// type index. `funcref` and the other legacy spellings are the nullable
// abstract forms: funcref is (ref null func).
struct ValueType {
    enum Kind : uint8_t {
        // Number types.
        i32, i64, f32, f64,
        // Abstract heap types. Everything from funcref on is a reference.
        funcref, externref, i31ref, exnref,
        structref, arrayref, anyref, eqref,
        nullref, nullfuncref, nullexternref,
        // A concrete heap type: typeidx names the type.
        ref,
    };

    constexpr ValueType(Kind kind = i32)
        : kind(kind), nullable(kind >= funcref), typeidx(0) {}
    constexpr ValueType(Kind kind, bool nullable)
        : kind(kind), nullable(nullable), typeidx(0) {}

    // (ref $t) / (ref null $t)
    static constexpr ValueType concrete(index_t typeidx, bool nullable){
        ValueType type(ref, nullable);
        type.typeidx = typeidx;
        return type;
    }

    Kind kind;
    bool nullable;   // meaningful for the reference kinds
    index_t typeidx; // meaningful when kind == ref

    constexpr bool is_num() const { return kind <= f64; }
    constexpr bool is_ref() const { return kind >= funcref; }
    constexpr bool is_concrete() const { return kind == ref; }
    // A bottom heap type: the null of some hierarchy, and nothing else.
    constexpr bool is_bottom() const { return kind >= nullref && kind <= nullexternref; }
};

constexpr bool operator==(const ValueType& a, const ValueType& b){
    if(a.kind != b.kind) return false;
    if(a.is_ref() && (a.nullable != b.nullable)) return false;
    return !a.is_concrete() || (a.typeidx == b.typeidx);
}
constexpr bool operator!=(const ValueType& a, const ValueType& b){ return !(a == b); }

// Full definitions (after Value is defined above)
struct StructObj : public GcObject {
    void gc_children(void (*visit)(GcObject*, void*), void* context) const override;
    void gc_clear_children() override;
    index_t typeidx;
    // One per field, in declaration order. A packed field is held as an i32
    // already truncated to its width; struct.get_s / _u extend it on the way
    // out.
    std::vector<Value> fields;
};

struct ArrayObj : public GcObject {
    void gc_children(void (*visit)(GcObject*, void*), void* context) const override;
    void gc_clear_children() override;
    index_t typeidx;
    std::vector<Value> elems; // packed elements as for StructObj::fields
};

// An exception payload is on the same heap: it holds Values, so a cycle can
// run through one -- catch a reference, then store the exnref back into it.
struct WasmException : public GcObject {
    void gc_children(void (*visit)(GcObject*, void*), void* context) const override;
    void gc_clear_children() override;
    index_t tag_addr;
    std::vector<Value> values;
    WasmException(index_t addr, std::vector<Value> vals) : tag_addr(addr), values(std::move(vals)) {}
};

struct FuncType {
    std::vector<ValueType> params;
    std::vector<ValueType> results;
};
bool operator==(const FuncType&, const FuncType&);

// ── Type definitions (GC proposal) ──────────────────────────────────────────
// A type section entry used to be a FuncType. It is now a subtype: a composite
// type -- function, struct or array -- that may name a supertype and may sit
// inside an explicit recursion group.

// Packed storage for struct fields and array elements. Never a value type: a
// packed field is read and written through i32.
enum class PackedType { i8, i16 };
using StorageType = std::variant<ValueType, PackedType>;

struct FieldType {
    StorageType storage;
    bool mut = false;
};
bool operator==(const FieldType&, const FieldType&);

struct StructType {
    std::vector<FieldType> fields;
};
bool operator==(const StructType&, const StructType&);

struct ArrayType {
    FieldType field;
};
bool operator==(const ArrayType&, const ArrayType&);

using CompType = std::variant<FuncType, StructType, ArrayType>;

struct SubType {
    SubType() = default;
    explicit SubType(FuncType type) : comp(std::move(type)) {}
    explicit SubType(StructType type) : comp(std::move(type)) {}
    explicit SubType(ArrayType type) : comp(std::move(type)) {}

    bool final = true;
    std::vector<index_t> supers; // the spec allows at most one; kept as written
    CompType comp;

    bool is_func()   const { return std::holds_alternative<FuncType>(comp); }
    bool is_struct() const { return std::holds_alternative<StructType>(comp); }
    bool is_array()  const { return std::holds_alternative<ArrayType>(comp); }
    // The function type, for the many places a type index can only ever name
    // a function's. Throws Exception::Exception when it names something else.
    const FuncType& func() const;
    FuncType& func();
};
bool operator==(const SubType&, const SubType&);

// An explicit `(rec ...)` group: `count` consecutive entries of
// WasmModule::types starting at `first`. An entry outside every group is a
// group of one by definition and is not recorded.
struct RecGroup {
    index_t first;
    index_t count;
};
bool operator==(const RecGroup&, const RecGroup&);

struct Limits {
    offset_t min;
    std::optional<offset_t> max;
    bool is64 = false;  // true = memory64 (i64 address space)
};

struct TableType {
    Limits limits;
    // Any reference type. It was one of two, which meant a table of concrete
    // references could not be declared at all.
    ValueType elemtype = ValueType(ValueType::funcref);
};

using MemType = Limits;

struct GlobalType {
    enum Mut {
        constant, variable
    };
    Mut mut;
    ValueType type;
};

// A tag's descriptor is a typeidx, the same shape as a function's. It is a
// distinct type so that an imported tag stays tellable apart from an imported
// function in WasmImport::desc.
struct TagType {
    index_t typeidx;
};

// Whether a runtime value could inhabit `type`, judged on representation
// alone. A number type and a reference kind with one representation give an
// exact answer; anyref, eqref and the bottom types are inhabited by several
// representations, and a concrete reference's type index is a static matter,
// so those are not refuted here. Used for the interpreter's own sanity
// checks, never as a substitute for validation.
bool value_matches(const Value& value, const ValueType& type);


// The spec writes a heap type as a signed integer: negative values are the
// abstract heap types, a non-negative one is a type index. These are the same
// numbers the binary format's s33 carries and the text parser already used.
constexpr int32_t heaptype_nofunc    = -13;
constexpr int32_t heaptype_noextern  = -14;
constexpr int32_t heaptype_none      = -15;
constexpr int32_t heaptype_func      = -16;
constexpr int32_t heaptype_extern    = -17;
constexpr int32_t heaptype_any       = -18;
constexpr int32_t heaptype_eq        = -19;
constexpr int32_t heaptype_i31       = -20;
constexpr int32_t heaptype_struct    = -21;
constexpr int32_t heaptype_array     = -22;
constexpr int32_t heaptype_exn       = -23;

// The abstract kind a negative heap type names, or nullopt when it names none.
std::optional<ValueType::Kind> abstract_heaptype(int32_t heaptype);
// The heap type of a reference: negative for an abstract kind, the type index
// for a concrete one.
int32_t heaptype_of(const ValueType& type);
// A reference type from a heap type as the spec writes it.
ValueType reference_to(int32_t heaptype, bool nullable);

// Whether `a` is a subtype of `b`, in a module whose type section is `types`.
// Structural, as the spec is: two composite types match when one's shape
// refines the other's, and a declared supertype is followed as well.
bool type_matches(const ValueType& a, const ValueType& b, const std::vector<SubType>& types);
bool storage_matches(const StorageType& a, const StorageType& b, const std::vector<SubType>& types);
bool field_matches(const FieldType& a, const FieldType& b, const std::vector<SubType>& types);
// Whether the composite type at index `a` is a subtype of the one at `b`,
// following any supertype `a` declares.
bool comp_matches(index_t a, index_t b, const std::vector<SubType>& types);
// The same, by shape alone: what a declared supertype has to satisfy for the
// declaration to be honest.
bool comp_refines(index_t a, index_t b, const std::vector<SubType>& types);

// The top of the hierarchy a reference type belongs to, as its nullable form:
// anyref, funcref, externref or exnref. A cast can only be attempted from
// there, since the hierarchies never mix.
ValueType top_of(const ValueType& type, const std::vector<SubType>& types);
// What `a` still admits once `b` is taken out of it. Only nullability is
// subtracted: a cast to a nullable type accounts for null, so what is left
// cannot be null.
ValueType reference_diff(const ValueType& a, const ValueType& b);

// The value a local, a global, a struct field or an array element of this type
// starts as: zero for a number, and the null of the right representation for a
// reference. `types` resolves a concrete reference to the shape it names.
Value default_value(const ValueType& type, const std::vector<SubType>& types);
Value default_value(const StorageType& storage, const std::vector<SubType>& types);

struct ExternVal {
    enum ExternType {Func, Mem, Table, Global, Tag} type;
    index_t addr;
};

}

template <> struct std::hash<WasmVM::ValueType> {
    std::size_t operator()(const WasmVM::ValueType& type) const;
};
template <> struct std::hash<WasmVM::FuncType> {
    std::size_t operator()(const WasmVM::FuncType& functype) const;
};
template <> struct std::hash<WasmVM::SubType> {
    std::size_t operator()(const WasmVM::SubType& type) const;
};

#endif