#ifndef WASMVM_PP_DEF
#define WASMVM_PP_DEF

#include <string>
#include <istream>
#include <structures/WasmModule.hpp>

namespace WasmVM {

WasmModule module_parse(std::string src);

}

#endif