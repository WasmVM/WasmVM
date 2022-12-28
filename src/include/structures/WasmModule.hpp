#ifndef WASMVM_PP_STRUCTURE_MODULE
#define WASMVM_PP_STRUCTURE_MODULE

#include <vector>
#include <Types.hpp>

namespace WasmVM {

struct WasmModule {
    std::vector<FuncType> types;
};

}

#endif