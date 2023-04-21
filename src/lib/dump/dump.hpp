#ifndef WASMVM_PP_DUMP_DEF
#define WASMVM_PP_DUMP_DEF

#include <ostream>
#include <WasmVM.hpp>

namespace WasmVM {

std::ostream& operator<<(std::ostream& stream, const ValueType& type);
std::ostream& operator<<(std::ostream& stream, const FuncType& type);
std::ostream& operator<<(std::ostream& stream, const WasmImport& import);
std::ostream& operator<<(std::ostream& stream, const WasmFunc& func);
std::ostream& operator<<(std::ostream& stream, const WasmInstr& instr);
std::ostream& operator<<(std::ostream& stream, const ConstInstr& instr);
std::ostream& operator<<(std::ostream& stream, const TableType& table);
std::ostream& operator<<(std::ostream& stream, const MemType& mem);
std::ostream& operator<<(std::ostream& stream, const WasmGlobal& global);
std::ostream& operator<<(std::ostream& stream, const WasmExport& export_);
std::ostream& operator<<(std::ostream& stream, const WasmElem& elem);
std::ostream& operator<<(std::ostream& stream, const WasmData& data);

}

#endif