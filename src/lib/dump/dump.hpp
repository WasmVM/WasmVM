#ifndef WASMVM_PP_DUMP_DEF
#define WASMVM_PP_DUMP_DEF

#include <ostream>
#include <WasmVM.hpp>

namespace WasmVM {

std::ostream& operator<<(std::ostream& stream, ValueType& type);
std::ostream& operator<<(std::ostream& stream, FuncType& type);
std::ostream& operator<<(std::ostream& stream, WasmImport& import);

}

#endif