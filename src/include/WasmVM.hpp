#ifndef WASMVM_PP_DEF
#define WASMVM_PP_DEF

#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
#include <structures/WasmModule.hpp>

#define VERSION "v1.1"

namespace WasmVM {

WasmModule module_parse(std::istream& stream);
std::ostream& module_dump(const WasmModule& module, std::ostream& stream);
std::ostream& module_encode(const WasmModule& module, std::ostream& stream);
WasmModule module_decode(std::istream& stream);

}

#endif