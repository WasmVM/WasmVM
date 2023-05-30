#ifndef WASMVM_EXEC_ModuleQueue_DEF
#define WASMVM_EXEC_ModuleQueue_DEF

#include <vector>
#include <filesystem>
#include <queue>
#include <map>
#include <optional>

#include <Types.hpp>

using namespace WasmVM;

struct ModuleQueue {
    struct Node {
        std::filesystem::path path;
        std::string name;
        size_t needs;
    };
    ModuleQueue(std::filesystem::path main_module, std::vector<std::filesystem::path> extra_paths, std::optional<std::filesystem::path> current_path = std::nullopt, std::optional<std::filesystem::path> system_path = std::nullopt);
    Node pop();
    inline bool empty() {
        return modules.empty();
    }

protected:
    std::queue<Node> modules;
    std::optional<std::filesystem::path> current_path;
    std::optional<std::filesystem::path> system_path;
    std::vector<std::filesystem::path> extra_paths;
    std::filesystem::path resolve(std::string module_name);
};

#endif