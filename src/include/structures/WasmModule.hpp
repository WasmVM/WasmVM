#ifndef WASMVM_PP_STRUCTURE_MODULE
#define WASMVM_PP_STRUCTURE_MODULE

#include <vector>
#include <string>
#include <Types.hpp>
#include <structures/WasmImport.hpp>
#include <structures/WasmFunc.hpp>

namespace WasmVM {

struct WasmModule {
    std::string id;
    std::vector<FuncType> types;
    std::vector<WasmImport> imports;
    std::vector<WasmFunc> funcs;
};

}

#endif