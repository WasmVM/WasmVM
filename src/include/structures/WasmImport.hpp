#ifndef WASMVM_PP_STRUCTURE_IMPORT
#define WASMVM_PP_STRUCTURE_IMPORT

#include <Types.hpp>

#include <string>
#include <variant>

namespace WasmVM {

struct WasmImport {
    std::string module;
    std::string name;
    std::variant<
        index_t, TableType, MemType, GlobalType
    > desc;
};

}

#endif