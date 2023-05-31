#ifndef WASMVM_PP_INSTANCES_DataInst_DEF
#define WASMVM_PP_INSTANCES_DataInst_DEF

#include <Types.hpp>
#include <vector>

namespace WasmVM {

struct DataInst {
    using Ref = std::variant<funcref_t, externref_t>;
    RefType type;
    std::vector<Ref> elem;
};

}

#endif