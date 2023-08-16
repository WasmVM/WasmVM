#ifndef WASMVM_PP_INSTANCES_ModuleInst_DEF
#define WASMVM_PP_INSTANCES_ModuleInst_DEF

#include <Types.hpp>
#include <vector>

#include "ExportInst.hpp"

namespace WasmVM {

struct ModuleInst {
    std::vector<FuncType> types;
    std::vector<index_t> funcaddrs;
    std::vector<index_t> tableaddrs;
    std::vector<index_t> memaddrs;
    std::vector<index_t> globaladdrs;
    std::vector<index_t> elemaddrs;
    std::vector<index_t> dataaddrs;
    std::vector<ExportInst> exports; 
};

}

#endif