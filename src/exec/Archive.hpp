#ifndef WASMVM_EXEC_Archive_DEF
#define WASMVM_EXEC_Archive_DEF

#include <filesystem>
#include <cstddef>
#include <vector>
#include <map>

namespace WasmVM {

struct Archive {

    constexpr static std::byte magic[4] {(std::byte)'V', (std::byte)'M', (std::byte)'A', (std::byte)'R'};
    constexpr static std::byte version[4] {(std::byte)0x00, (std::byte)0x01, (std::byte)0x02, (std::byte)0x00};

    Archive(std::filesystem::path path);

    void create(std::map<std::filesystem::path, std::filesystem::path> modules, std::filesystem::path prefix = "");
    std::filesystem::path extract(std::string module, std::filesystem::path prefix = "");
    std::vector<std::filesystem::path> list(std::filesystem::path prefix = "");

private:
    std::filesystem::path path;

};  // namespace Archive

} // namespace WasmVM

#endif