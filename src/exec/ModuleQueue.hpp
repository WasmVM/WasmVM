#ifndef WASMVM_EXEC_ModuleQueue_DEF
#define WASMVM_EXEC_ModuleQueue_DEF

#include <vector>
#include <filesystem>
#include <queue>
#include <map>
#include <optional>

#include <WasmVM.hpp>

using namespace WasmVM;

struct ModuleQueue {
    struct Node {
        std::filesystem::path file_path;
        WasmModule module;
        std::vector<std::filesystem::path> import_paths;
    };
    ModuleQueue(std::filesystem::path main_module,
        std::map<std::filesystem::path, ModuleInst>& module_insts,
        std::vector<std::filesystem::path> extra_paths,
        std::optional<std::filesystem::path> system_path = std::nullopt,
        bool check_parent = true,
        bool validate = true
    );
    Node pop();
    inline bool empty() {
        return modules.empty();
    }

protected:
    std::queue<Node> modules;
    bool check_parent;
    std::map<std::filesystem::path, ModuleInst>& module_insts;
    std::optional<std::filesystem::path> system_path;
    std::vector<std::filesystem::path> extra_paths;
    std::filesystem::path resolve(std::string module_name, std::filesystem::path parent);
};

#endif