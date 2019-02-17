#include "Allocates.h"

#include <stdlib.h>
#include <dataTypes/Value.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

int allocate_Module(WasmModule* module, Store* store, ModuleInst** allocated)
{
    // TODO:
    return 0;
}
uint32_t allocate_Function(WasmFunc* func, Store* store, ModuleInst* moduleInst)
{
    FuncInst* funcInst = new_FuncInst(moduleInst, (FuncType*)moduleInst->types->at(moduleInst->types, func->type));
    for(size_t i = 0; i < func->locals->length; ++i) {
        ValueType* local = (ValueType*) malloc(sizeof(ValueType));
        *local = *(ValueType*)func->locals->at(func->locals, i);
        funcInst->locals->push_back(funcInst->locals, local);
    }
    for(size_t i = 0; i < func->body->size; ++i) {
        funcInst->code->push_back(funcInst->code, allocate_Instruction((WasmInstr*)func->body->at(func->body, i)));
    }
    uint32_t address = store->funcs->length;
    store->funcs->push_back(store->funcs, funcInst);
    return address;
}
uint32_t allocate_Table(WasmTable* table, Store* store)
{
    TableInst* tableInst = new_TableInst();
    tableInst->elem->resize(tableInst->elem, table->min);
    tableInst->max = table->max;
    uint32_t address = store->tables->length;
    store->tables->push_back(store->tables, tableInst);
    return address;
}
uint32_t allocate_Memory(WasmMemory* memory, Store* store)
{
    MemInst* memInst = new_MemInst();
    memInst->data->resize(memInst->data, memory->min);
    memInst->max = memory->max;
    uint32_t address = store->mems->length;
    store->mems->push_back(store->mems, memInst);
    return address;
}
uint32_t allocate_Global(WasmGlobal* global, Store* store)
{
    GlobalInst* globalInst = new_GlobalInst();
    globalInst->mut = global->mut;
    globalInst->value.parent.entryType = Entry_Value;
    globalInst->value.type = global->valType;
    globalInst->value.value = global->init.value;
    uint32_t address = store->globals->length;
    store->globals->push_back(store->globals, globalInst);
    return address;
}