#ifndef WASMVM_PP_INSTANCES_ModuleInst_DEF
#define WASMVM_PP_INSTANCES_ModuleInst_DEF

#include <Types.hpp>
#include <vector>
#include <structures/WasmNames.hpp>

#include "ExportInst.hpp"

namespace WasmVM {

struct ModuleInst {
    std::vector<SubType> types;
    // The Store-wide CanonId of each local type index, filled at
    // instantiation. `type_ids[i]` is the identity of `types[i]`.
    std::vector<CanonId> type_ids;
    std::vector<index_t> funcaddrs;
    std::vector<index_t> tableaddrs;
    std::vector<index_t> memaddrs;
    std::vector<index_t> globaladdrs;
    std::vector<index_t> elemaddrs;
    std::vector<index_t> dataaddrs;
    std::vector<index_t> tagaddrs;
    std::vector<ExportInst> exports; 
    // Copied from the WasmModule's name section at instantiation.
    WasmNames names;

    void reset();
};

}

#endif