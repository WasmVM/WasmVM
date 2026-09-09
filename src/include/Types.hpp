#ifndef WASMVM_PP_TYPES_DEF
#define WASMVM_PP_TYPES_DEF

#include <vector>
#include <variant>
#include <optional>
#include <utility>
#include <string>
#include <cstdint>
#include <cstddef>
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

// Forward declaration for exnref_t
struct WasmException;

struct exnref_t {
    std::shared_ptr<WasmException> value; // nullptr = null exnref
    constexpr exnref_t() : value() {}
    explicit exnref_t(std::shared_ptr<WasmException> e) : value(std::move(e)) {}
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

// A struct or array on the GC heap. Held by shared_ptr, like exnref_t: a
// reference is null when the pointer is. Reference counting reclaims what it
// can; a cycle through a mutable reference field is not collected.
struct StructObj;
struct ArrayObj;
using structref_t = std::shared_ptr<StructObj>;
using arrayref_t = std::shared_ptr<ArrayObj>;

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
struct StructObj {
    index_t typeidx;
    // One per field, in declaration order. A packed field is held as an i32
    // already truncated to its width; struct.get_s / _u extend it on the way
    // out.
    std::vector<Value> fields;
};

struct ArrayObj {
    index_t typeidx;
    std::vector<Value> elems; // packed elements as for StructObj::fields
};

struct WasmException {
    index_t tag_addr;
    std::vector<Value> values;
    WasmException(index_t addr, std::vector<Value> vals) : tag_addr(addr), values(std::move(vals)) {}
};

// The element type of a table or an elem segment. Still only the two legacy
// reference types: tables holding a concrete reference are a further step.
enum class RefType {
    funcref = ValueType::funcref,
    externref = ValueType::externref,
};

using Ref = std::variant<funcref_t, externref_t>;

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
    RefType reftype;
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

// A table's / elem segment's element type as a value type, and back. The two
// legacy reference types are the nullable abstract forms, so the round trip is
// exact for them and meaningless for anything else.
constexpr ValueType valtype_of(RefType reftype){
    return ValueType((ValueType::Kind)reftype);
}
constexpr bool is_legacy_reftype(const ValueType& type){
    return type.nullable
        && ((type.kind == ValueType::funcref) || (type.kind == ValueType::externref));
}
constexpr RefType reftype_of(const ValueType& type){
    return (RefType)type.kind;
}

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