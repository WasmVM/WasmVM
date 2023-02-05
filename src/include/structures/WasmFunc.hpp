#ifndef WASMVM_PP_STRUCTURE_FUNC
#define WASMVM_PP_STRUCTURE_FUNC

#include <Types.hpp>

#include <vector>

namespace WasmVM {

struct WasmFunc {
    index_t typeidx;
    std::vector<ValueType> locals;
};

}

#endif