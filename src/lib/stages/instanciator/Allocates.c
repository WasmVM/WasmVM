#include "Allocates.h"

#include <stdlib.h>
#include <dataTypes/Value.h>
#include <instance/FuncInst.h>

int allocate_Module(WasmModule* module, Store* store, ModuleInst** allocated)
{
    // TODO:
    return 0;
}
FuncInst* allocate_Function(WasmFunc* func, Store* store, ModuleInst* moduleInst)
{
    FuncInst* funcInst = new_FuncInst(moduleInst, (FuncType*)moduleInst->types->at(moduleInst->types, func->type));
    for(size_t i = 0; i < func->locals->length; ++i) {
        ValueType* local = (ValueType*) malloc(sizeof(ValueType));
        *local = *(ValueType*)func->locals->at(func->locals, i);
        funcInst->locals->push_back(funcInst->locals, local);
    }
    return funcInst;
}
int allocate_Table(WasmTable* table, Store* store, FuncInst** allocated)
{
    // TODO:
    return 0;
}
int allocate_Memory(WasmMemory* memory, Store* store, MemInst** allocated)
{
    // TODO:
    return 0;
}
int allocate_Global(WasmGlobal* global, Store* store, GlobalInst** allocated)
{
    // TODO:
    return 0;
}