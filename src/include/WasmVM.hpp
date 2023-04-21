#ifndef WASMVM_PP_DEF
#define WASMVM_PP_DEF

#include <string>
#include <vector>
#include <cstddef>
#include <structures/WasmModule.hpp>

namespace WasmVM {

WasmModule module_parse(const std::string src);
std::string module_dump(const WasmModule& module);
std::vector<std::byte> module_encode(const WasmModule& module);

}

#endif