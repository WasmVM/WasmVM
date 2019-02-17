#ifndef WASMVM_INSTANCIATOR_ALLOCATES_DEF
#define WASMVM_INSTANCIATOR_ALLOCATES_DEF

#include <core/Store.h>
#include <structures/WasmModule.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

int allocate_Module(WasmModule* module, Store* store, ModuleInst** allocated);
int allocate_Function(WasmFunc* func, Store* store, ModuleInst* moduleInst, FuncInst** allocated);
int allocate_Table(WasmTable* table, Store* store, FuncInst** allocated);
int allocate_Memory(WasmMemory* memory, Store* store, MemInst** allocated);
int allocate_Global(WasmGlobal* global, Store* store, GlobalInst** allocated);

#endif