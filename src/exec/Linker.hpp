#ifndef WASMVM_EXEC_Linker_DEF
#define WASMVM_EXEC_Linker_DEF

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <variant>
#include <utility>
#include <optional>
#include <functional>
#include <WasmVM.hpp>

namespace WasmVM {

struct Linker {

    void consume(std::filesystem::path module_path);
    WasmModule get();

    struct ExternEntry {
        index_t module;
        index_t address;
    };

private:

    struct IndexEntry {
        index_t index;
        enum {Import, Address} kind;
    };
    
    using Descriptor = std::variant<IndexEntry, WasmImport, ExternEntry>;
    
    struct ModuleEntry {
        std::filesystem::path path;
        std::vector<index_t> types;
        std::vector<Descriptor> funcs;
        std::vector<Descriptor> tables;
        std::vector<Descriptor> mems;
        std::vector<Descriptor> globals;
        std::vector<index_t> elems;
        std::vector<index_t> datas;
        std::vector<WasmExport> exports;
        std::optional<index_t> start;
    };

    std::vector<ModuleEntry> modules;

    struct {
        index_t func = 0;
        index_t table = 0;
        index_t mem = 0;
        index_t global = 0;
    } import_counter;

    std::unordered_map<FuncType, index_t> type_map;
    std::unordered_map<std::string, std::unordered_map<std::string, index_t>> import_map; // Map import name to output address

    struct {
        std::vector<index_t> funcs;
        std::vector<index_t> globals;
        std::vector<index_t> elems;
        std::vector<index_t> datas;
    } module_index_list;

    WasmModule output;

    void resolve_imports(std::unordered_map<std::string, std::unordered_map<std::string, ExternEntry>> &export_map, std::vector<Descriptor> &descs);
    void instr_update_indices(WasmInstr& instr, ModuleEntry& module_entry);
};

}

template <> struct std::hash<WasmVM::Linker::ExternEntry> {
    std::size_t operator()(const WasmVM::Linker::ExternEntry&) const;
};

template <> struct std::equal_to<WasmVM::Linker::ExternEntry> {
    bool operator()(const WasmVM::Linker::ExternEntry&, const WasmVM::Linker::ExternEntry&) const;
};

#endif