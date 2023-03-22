#ifndef WASMVM_PP_STRUCTURE_EXPORT
#define WASMVM_PP_STRUCTURE_EXPORT

#include <Types.hpp>

#include <string>
#include <variant>

namespace WasmVM {

struct WasmExport {
    enum class DescType {func, table, mem, global};
    
    WasmExport(std::string name, DescType desc, index_t index) : name(name), desc(desc), index(index){}
    
    std::string name;
    DescType desc;
    index_t index;
};

}

#endif