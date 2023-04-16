#ifndef WASMVM_PP_STRUCTURE_DATA
#define WASMVM_PP_STRUCTURE_DATA

#include <Types.hpp>
#include <structures/WasmInstr.hpp>

#include <vector>
#include <optional>

namespace WasmVM {

struct WasmData {
    
    struct DataMode {
        enum class Mode { passive, active } type;
        std::optional<index_t> memidx;
        std::optional<ConstInstr> offset;
    };
    
    std::vector<byte_t> init;
    DataMode mode;
};

}

#endif