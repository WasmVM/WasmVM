#ifndef WASMVM_PP_INSTANCES_ElemInst_DEF
#define WASMVM_PP_INSTANCES_ElemInst_DEF

#include <Types.hpp>
#include <vector>

namespace WasmVM {

struct ElemInst {
    using Ref = std::variant<funcref_t, externref_t>;
    RefType type;
    std::vector<Ref> elem;
};

}

#endif