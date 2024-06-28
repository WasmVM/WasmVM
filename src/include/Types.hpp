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

constexpr u64_t page_size = 65536;

using Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t>;

enum ValueType : u8_t {
    i32 = 0x7f, i64 = 0x7e, f32 = 0x7d, f64 = 0x7c, funcref = 0x70, externref = 0x6f
};

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

struct Limits {
    offset_t min;
    std::optional<offset_t> max;
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

struct ExternVal {
    enum ExternType {Func, Mem, Table, Global} type;
    index_t addr;
};

}

template <> struct std::hash<WasmVM::FuncType> {
    std::size_t operator()(const WasmVM::FuncType& functype) const;
};

#endif