#ifndef WASMVM_EXEC_Linker_DEF
#define WASMVM_EXEC_Linker_DEF

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <variant>
#include <utility>
#include <WasmVM.hpp>

namespace WasmVM {

struct Linker {

    void consume(std::filesystem::path module_path);
    WasmModule get();

private:
    struct ImportCounter {
        index_t func = 0;
        index_t table = 0;
        index_t mem = 0;
        index_t global = 0;
    };
    
    using ExternEntry = std::pair<size_t, index_t>; // (module_index, desc_index)
    using IndexEntry = std::variant<index_t, WasmImport, ExternEntry>; // resolved_index | import | extern_entry
    
    struct ModuleEntry {
        std::filesystem::path path;
        std::vector<index_t> types;
        std::vector<IndexEntry> funcs;
        std::unordered_map<std::string, std::pair<ExternVal::ExternType, index_t>> exports;
        ImportCounter counter;
    };

    std::unordered_map<FuncType, index_t> typemap;
    std::vector<ModuleEntry> entries;
    std::unordered_map<std::string, std::unordered_map<std::string, index_t>> resolved; // resolved imports (module_name, (import_names, desc_index))

    WasmModule output;
    ImportCounter counter;
};

}

#endif