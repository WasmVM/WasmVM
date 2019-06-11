#include "Allocates.h"

#include <stdlib.h>
#include <string.h>
#include <dataTypes/Value.h>
#include <dataTypes/FuncType.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>
#include <instance/ExportInst.h>
#include <instance/ModuleInst.h>

ModuleInst* allocate_Module(WasmModule* module, Store store, ExportInst* exportInsts[], size_t exportCount)
{
    char* moduleName = (char*) malloc(sizeof(char) * (strlen(module->module_name) + 1));
    strcpy(moduleName, module->module_name);
    ModuleInst* moduleInst = new_ModuleInst(moduleName);
    // Copy types
    for(size_t i = 0; i < vector_size(module->types); ++i) {
        FuncType type = vector_at(FuncType, module->types, i);
        FuncType newType = new_FuncType();
        for(size_t j = 0; j < vector_size(type->params); ++j) {
            vector_push_back(newType->params, vector_at(ValueType*, type->params, j));
        }
        for(size_t j = 0; j < vector_size(type->results); ++j) {
            vector_push_back(newType->results, vector_at(ValueType*, type->results, j));
        }
        vector_push_back(moduleInst->types, newType);
        free(newType);
    }
    // Extract imported exportInsts
    for(size_t i = 0; i < exportCount; ++i) {
        switch (exportInsts[i]->descType) {
            case Desc_Func:
                vector_push_back(moduleInst->funcaddrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Global:
                vector_push_back(moduleInst->globaladdrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Mem:
                vector_push_back(moduleInst->memaddrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Table:
                vector_push_back(moduleInst->tableaddrs, &exportInsts[i]->valueAddr);
                break;
            default:
                break;
        }
    }
    // Allocate new instances
    for(size_t i = 0; i < vector_size(module->funcs); ++i) {
        uint32_t address = allocate_Function(vector_at(WasmFunc*, module->funcs, i), store, moduleInst);
        vector_push_back(moduleInst->funcaddrs, &address);
    }
    for(size_t i = 0; i < vector_size(module->tables); ++i) {
        uint32_t address = allocate_Table(vector_at(WasmTable*, module->tables, i), store);
        vector_push_back(moduleInst->tableaddrs, &address);
    }
    for(size_t i = 0; i < vector_size(module->mems); ++i) {
        uint32_t address = allocate_Memory(vector_at(WasmMemory*, module->mems, i), store);
        vector_push_back(moduleInst->memaddrs, &address);
    }
    for(size_t i = 0; i < vector_size(module->globals); ++i) {
        uint32_t address = allocate_Global(vector_at(WasmGlobal*, module->globals, i), store);
        vector_push_back(moduleInst->globaladdrs, &address);
    }
    // Exports
    for(size_t i = 0; i < vector_size(module->exports); ++i) {
        WasmExport* export = vector_at(WasmExport*, module->exports, i);
        ExportInst* exportInst = new_ExportInst();
        exportInst->name = (char*) malloc(sizeof(char) * (strlen(export->name) + 1));
        strcpy(exportInst->name, export->name);
        exportInst->descType = export->descType;
        switch (export->descType) {
            case Desc_Func:
                exportInst->valueAddr = *vector_at(uint32_t*, moduleInst->funcaddrs, export->descIdx);
                break;
            case Desc_Global:
                exportInst->valueAddr = *vector_at(uint32_t*, moduleInst->globaladdrs, export->descIdx);
                break;
            case Desc_Mem:
                exportInst->valueAddr = *vector_at(uint32_t*, moduleInst->memaddrs, export->descIdx);
                break;
            case Desc_Table:
                exportInst->valueAddr = *vector_at(uint32_t*, moduleInst->tableaddrs, export->descIdx);
                break;
            default:
                break;
        }
        vector_push_back(moduleInst->exports, exportInst);
    }
    return moduleInst;
}
uint32_t allocate_Function(WasmFunc* func, Store store, ModuleInst* moduleInst)
{
    FuncInst* funcInst = new_FuncInst(moduleInst, vector_at(FuncType, moduleInst->types, func->type));
    for(size_t i = 0; i < vector_size(func->locals); ++i) {
        ValueType* local = (ValueType*) malloc(sizeof(ValueType));
        *local = *vector_at(ValueType*, func->locals, i);
        vector_push_back(funcInst->locals, local);
    }
    for(size_t i = 0; i < list_size(func->body); ++i) {
        list_push_back(funcInst->code, allocate_Instruction(func->body, i));
    }
    uint32_t address = vector_size(store->funcs);
    vector_push_back(store->funcs, funcInst);
    return address;
}
uint32_t allocate_Table(WasmTable* table, Store store)
{
    TableInst* tableInst = new_TableInst();
    vector_resize(tableInst->elem, table->min);
    tableInst->max = table->max;
    uint32_t address = vector_size(store->tables);
    vector_push_back(store->tables, tableInst);
    return address;
}
uint32_t allocate_Memory(WasmMemory* memory, Store store)
{
    MemInst* memInst = new_MemInst();
    vector_resize(memInst->data, memory->min * 65536);
    memset(vector_data(void*, memInst->data), 0, memory->min * 65536 * sizeof(char));
    memInst->max = memory->max;
    uint32_t address = vector_size(store->mems);
    vector_push_back(store->mems, memInst);
    return address;
}
uint32_t allocate_Global(WasmGlobal* global, Store store)
{
    GlobalInst* globalInst = new_GlobalInst();
    globalInst->mut = global->mut;
    globalInst->value.parent.entryType = Entry_Value;
    globalInst->value.type = global->valType;
    globalInst->value.value = global->init.value;
    uint32_t address = vector_size(store->globals);
    vector_push_back(store->globals, globalInst);
    return address;
}