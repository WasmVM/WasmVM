#ifndef WASMVM_PP_INSTANCES_GlobalInst_DEF
#define WASMVM_PP_INSTANCES_GlobalInst_DEF

#include <Types.hpp>

namespace WasmVM {

struct GlobalInst {
    ValueType type;
    Value value;
};

}

#endif