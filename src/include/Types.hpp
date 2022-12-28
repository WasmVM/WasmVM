#ifndef WASMVM_PP_DATATYPES_VALUE_DEF
#define WASMVM_PP_DATATYPES_VALUE_DEF

#include <vector>
#include <variant>
#include <optional>
#include <utility>

namespace WasmVM {

using u32_t = uint32_t;
using u64_t = uint64_t;
using i32_t = int32_t;
using i64_t = int64_t;
using f32_t = float;
using f64_t = double;
using index_t = uint32_t;
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

struct FuncType {
    std::vector<ValueType> params;
    std::vector<ValueType> results;
};

}

#endif