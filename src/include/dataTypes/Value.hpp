#ifndef WASMVM_PP_DATATYPES_VALUE_DEF
#define WASMVM_PP_DATATYPES_VALUE_DEF

#include <variant>
#include <any>

enum class ValueType {
    i32, i64, f32, f64, ref
};

using Value = std::variant<int32_t, int64_t, float, double, std::any>;

#endif