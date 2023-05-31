#ifndef WASMVM_PP_INSTANCES_MemInst_DEF
#define WASMVM_PP_INSTANCES_MemInst_DEF

#include <Types.hpp>
#include <vector>

namespace WasmVM {

struct MemInst {
    MemType type;
    std::vector<byte_t> data;
};

}

#endif