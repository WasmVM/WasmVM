#ifndef WASMVM_PP_TYPES_DEF
#define WASMVM_PP_TYPES_DEF

#include <vector>
#include <variant>
#include <optional>
#include <utility>
#include <string>
#include <cstdint>

namespace WasmVM {

using u32_t = uint32_t;
using u64_t = uint64_t;
using i32_t = int32_t;
using i64_t = int64_t;
using f32_t = float;
using f64_t = double;

using u8_t = uint8_t;
using i8_t = int8_t;
using u16_t = uint16_t;
using i16_t = int16_t;

using index_t = uint32_t;
using offset_t = u64_t;
using align_t = uint8_t;
constexpr index_t index_npos = (index_t) -1;
using funcref_t = std::optional<index_t>;
using externref_t = void*;
static_assert(sizeof(f32_t) == sizeof(i32_t));
static_assert(sizeof(f64_t) == sizeof(i64_t));

using Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t>;

enum ValueType {
    i32, i64, f32, f64, funcref, externref
};

static_assert(Value(std::in_place_type<i32_t>).index() == ValueType::i32);
static_assert(Value(std::in_place_type<i64_t>).index() == ValueType::i64);
static_assert(Value(std::in_place_type<f32_t>).index() == ValueType::f32);
static_assert(Value(std::in_place_type<f64_t>).index() == ValueType::f64);
static_assert(Value(std::in_place_type<funcref_t>).index() == ValueType::funcref);
static_assert(Value(std::in_place_type<externref_t>).index() == ValueType::externref);

enum class RefType {
    funcref = ValueType::funcref,
    externref,
};

struct FuncType {
    std::vector<ValueType> params;
    std::vector<ValueType> results;

    bool operator==(const FuncType&);
    bool operator!=(const FuncType& op) {
        return !operator==(op);
    }
};

struct Limits {
    u32_t min;
    std::optional<u32_t> max;
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

}

#endif