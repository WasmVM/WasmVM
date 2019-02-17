#ifndef WASMVM_INSTANCIATOR_ALLOCATES_DEF
#define WASMVM_INSTANCIATOR_ALLOCATES_DEF

#include <core/Store.h>
#include <structures/WasmModule.h>
#include <structures/WasmTable.h>
#include <structures/WasmMemory.h>
#include <structures/WasmGlobal.h>
#include <structures/instrs/WasmInstr.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>
#include <instance/InstrInst.h>

int allocate_Module(WasmModule* module, Store* store, ModuleInst** allocated);
InstrInst* allocate_Instruction(WasmInstr* instr);
uint32_t allocate_Function(WasmFunc* func, Store* store, ModuleInst* moduleInst);
uint32_t allocate_Table(WasmTable* table, Store* store);
uint32_t allocate_Memory(WasmMemory* memory, Store* store);
uint32_t allocate_Global(WasmGlobal* global, Store* store);

#endif