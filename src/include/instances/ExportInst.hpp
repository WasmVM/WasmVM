#ifndef WASMVM_PP_INSTANCES_ExportInst_DEF
#define WASMVM_PP_INSTANCES_ExportInst_DEF

#include <Types.hpp>
#include <string>

namespace WasmVM {

struct ExportInst {
    std::string name;
    ExternVal value;
};

}

#endif