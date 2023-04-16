#ifndef WASMVM_PP_DUMP_DEF
#define WASMVM_PP_DUMP_DEF

#include <ostream>
#include <WasmVM.hpp>

namespace WasmVM {

std::ostream& operator<<(std::ostream& stream, ValueType& type);
std::ostream& operator<<(std::ostream& stream, FuncType& type);
std::ostream& operator<<(std::ostream& stream, WasmImport& import);
std::ostream& operator<<(std::ostream& stream, WasmFunc& func);
std::ostream& operator<<(std::ostream& stream, WasmInstr& instr);
std::ostream& operator<<(std::ostream& stream, ConstInstr& instr);
std::ostream& operator<<(std::ostream& stream, TableType& table);
std::ostream& operator<<(std::ostream& stream, WasmExport& export_);
std::ostream& operator<<(std::ostream& stream, WasmElem& elem);
std::ostream& operator<<(std::ostream& stream, WasmData& data);

}

#endif