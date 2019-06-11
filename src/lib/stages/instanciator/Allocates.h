#ifndef WASMVM_INSTANCIATOR_ALLOCATES_DEF
#define WASMVM_INSTANCIATOR_ALLOCATES_DEF

#include <stdlib.h>
#include <core/Store.h>
#include <dataTypes/list_p.h>
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
#include <instance/ExportInst.h>

ModuleInst* allocate_Module(WasmModule* module, Store store, ExportInst* exportInsts[], size_t exportCount);
InstrInst* allocate_Instruction(list_p funcBody, size_t index);
uint32_t allocate_Function(WasmFunc* func, Store store, ModuleInst* moduleInst);
uint32_t allocate_Table(WasmTable* table, Store store);
uint32_t allocate_Memory(WasmMemory* memory, Store store);
uint32_t allocate_Global(WasmGlobal* global, Store store);

#endif