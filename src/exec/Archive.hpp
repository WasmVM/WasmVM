#ifndef WASMVM_EXEC_Archive_DEF
#define WASMVM_EXEC_Archive_DEF

#include <filesystem>
#include <cstddef>
#include <vector>
#include <map>
#include <optional>

namespace WasmVM {

struct Archive {

    constexpr static char magic[4] {'V', 'M', 'A', 'R'};
    constexpr static char version[4] {0x00, 0x01, 0x02, 0x00};

    Archive(std::filesystem::path path);

    void create(std::map<std::filesystem::path, std::filesystem::path> modules, std::filesystem::path prefix = "");
    std::optional<std::filesystem::path> extract(std::filesystem::path module, std::filesystem::path prefix = "");
    std::vector<std::filesystem::path> list(std::filesystem::path prefix = "");

private:
    std::filesystem::path path;

};  // namespace Archive

} // namespace WasmVM

#endif