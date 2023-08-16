#ifndef WASMVM_PP_INSTANCES_ElemInst_DEF
#define WASMVM_PP_INSTANCES_ElemInst_DEF

#include <Types.hpp>
#include <vector>

namespace WasmVM {

struct ElemInst {
    RefType type;
    std::vector<Ref> elem;
};

}

#endif