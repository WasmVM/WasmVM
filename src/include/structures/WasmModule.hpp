#ifndef WASMVM_PP_STRUCTURE_MODULE
#define WASMVM_PP_STRUCTURE_MODULE

#include <vector>
#include <string>
#include <Types.hpp>

namespace WasmVM {

struct WasmModule {
    std::string id;
    std::vector<FuncType> types;
};

}

#endif