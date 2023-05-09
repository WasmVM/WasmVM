#ifndef WASMVM_PP_STRUCTURE_EXPORT
#define WASMVM_PP_STRUCTURE_EXPORT

#include <Types.hpp>

#include <string>

namespace WasmVM {

struct WasmExport {
    enum class DescType {func = 0x00, table = 0x01, mem = 0x02, global = 0x03};
    
    std::string name;
    DescType desc;
    index_t index;
};

}

#endif