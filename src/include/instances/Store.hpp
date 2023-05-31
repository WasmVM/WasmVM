#ifndef WASMVM_PP_INSTANCES_STORE_DEF
#define WASMVM_PP_INSTANCES_STORE_DEF

#include <Types.hpp>
#include <vector>

#include "FuncInst.hpp"
#include "TableInst.hpp"
#include "MemInst.hpp"
#include "GlobalInst.hpp"
#include "ElemInst.hpp"
#include "DataInst.hpp"

namespace WasmVM {

struct Store {
    std::vector<FuncInst> funcs;
    std::vector<TableInst> tables;
    std::vector<MemInst> mems;
    std::vector<GlobalInst> globals;
    std::vector<ElemInst> elems;
    std::vector<DataInst> datas;
};

}

#endif