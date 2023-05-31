#ifndef WASMVM_PP_INSTANCES_TableInst_DEF
#define WASMVM_PP_INSTANCES_TableInst_DEF

#include <Types.hpp>
#include <vector>
#include <variant>

namespace WasmVM {

struct TableInst {
    using Ref = std::variant<funcref_t, externref_t>;
    TableType type;
    std::vector<Ref> elems;
};

}

#endif