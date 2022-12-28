#ifndef WASMVM_PP_DEF
#define WASMVM_PP_DEF

#include <string>
#include <istream>
#include <structures/WasmModule.hpp>

namespace WasmVM {

WasmModule module_parse(std::string text);
WasmModule module_parse(std::istream& stream);

}

#endif