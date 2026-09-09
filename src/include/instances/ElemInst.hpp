#ifndef WASMVM_PP_INSTANCES_ElemInst_DEF
#define WASMVM_PP_INSTANCES_ElemInst_DEF

#include <Types.hpp>
#include <vector>

namespace WasmVM {

struct ElemInst {
    ValueType type = ValueType(ValueType::funcref);
    std::vector<Value> elem;
};

}

#endif