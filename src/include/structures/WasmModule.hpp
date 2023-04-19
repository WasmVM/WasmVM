#ifndef WASMVM_PP_STRUCTURE_MODULE
#define WASMVM_PP_STRUCTURE_MODULE

#include <vector>
#include <string>
#include <optional>
#include <Types.hpp>
#include <structures/WasmImport.hpp>
#include <structures/WasmFunc.hpp>
#include <structures/WasmExport.hpp>
#include <structures/WasmElem.hpp>
#include <structures/WasmData.hpp>
#include <structures/WasmGlobal.hpp>

namespace WasmVM {

struct WasmModule {
    std::vector<FuncType> types;
    std::vector<WasmImport> imports;
    std::vector<WasmFunc> funcs;
    std::vector<TableType> tables;
    std::vector<MemType> mems;
    std::vector<WasmGlobal> globals;
    std::vector<WasmExport> exports;
    std::optional<index_t> start;
    std::vector<WasmElem> elems;
    std::vector<WasmData> datas;
};

}

#endif