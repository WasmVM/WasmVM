#ifndef WASMVM_PP_DATATYPES_VALUE_DEF
#define WASMVM_PP_DATATYPES_VALUE_DEF

#include <vector>
#include <variant>
#include <any>

namespace WasmVM {

enum class ValueType {
    i32, i64, f32, f64, ref
};

using U32 = uint32_t;
using U64 = uint64_t;
using I32 = int32_t;
using I64 = int64_t;
using F32 = float;
using F64 = double;
using Ref = std::any;

using Value = std::variant<U32, U64, I32, I64, F32, F64, Ref>;

struct FuncType {
    std::vector<ValueType> params;
    std::vector<ValueType> results;
};

}

#endif