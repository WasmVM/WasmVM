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

ModuleInst* allocate_Module(WasmModule* module, Store* store, ExportInst* exportInsts[], size_t exportCount)
{
    char* moduleName = (char*) malloc(sizeof(char) * (strlen(module->module_name) + 1));
    strcpy(moduleName, module->module_name);
    ModuleInst* moduleInst = new_ModuleInst(moduleName);
    // Copy types
    for(size_t i = 0; i < module->types->length; ++i) {
        FuncType* type = (FuncType*) module->types->at(module->types, i);
        FuncType* newType = new_FuncType();
        for(size_t j = 0; j < type->params->length; ++j) {
            newType->params->push_back(newType->params, type->params->at(type->params, j));
        }
        for(size_t j = 0; j < type->results->length; ++j) {
            newType->results->push_back(newType->results, type->results->at(type->results, j));
        }
        moduleInst->types->push_back(moduleInst->types, newType);
        free(newType);
    }
    // Extract imported exportInsts
    for(size_t i = 0; i < exportCount; ++i) {
        switch (exportInsts[i]->descType) {
            case Desc_Func:
                moduleInst->funcaddrs->push_back(moduleInst->funcaddrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Global:
                moduleInst->globaladdrs->push_back(moduleInst->globaladdrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Mem:
                moduleInst->memaddrs->push_back(moduleInst->memaddrs, &exportInsts[i]->valueAddr);
                break;
            case Desc_Table:
                moduleInst->tableaddrs->push_back(moduleInst->tableaddrs, &exportInsts[i]->valueAddr);
                break;
            default:
                break;
        }
    }
    // Allocate new instances
    for(size_t i = 0; i < module->funcs->length; ++i) {
        uint32_t address = allocate_Function((WasmFunc*)module->funcs->at(module->funcs, i), store, moduleInst);
        moduleInst->funcaddrs->push_back(moduleInst->funcaddrs, &address);
    }
    for(size_t i = 0; i < module->tables->length; ++i) {
        uint32_t address = allocate_Table((WasmTable*)module->tables->at(module->tables, i), store);
        moduleInst->tableaddrs->push_back(moduleInst->tableaddrs, &address);
    }
    for(size_t i = 0; i < module->mems->length; ++i) {
        uint32_t address = allocate_Memory((WasmMemory*)module->mems->at(module->mems, i), store);
        moduleInst->memaddrs->push_back(moduleInst->memaddrs, &address);
    }
    for(size_t i = 0; i < module->globals->length; ++i) {
        uint32_t address = allocate_Global((WasmGlobal*)module->globals->at(module->globals, i), store);
        moduleInst->globaladdrs->push_back(moduleInst->globaladdrs, &address);
    }
    // Exports
    for(size_t i = 0; i < module->exports->length; ++i) {
        WasmExport* export = (WasmExport*)module->exports->at(module->exports, i);
        ExportInst* exportInst = new_ExportInst();
        exportInst->name = (char*) malloc(sizeof(char) * (strlen(export->name) + 1));
        strcpy(exportInst->name, export->name);
        exportInst->descType = export->descType;
        switch (export->descType) {
            case Desc_Func:
                exportInst->valueAddr = *(uint32_t*)moduleInst->funcaddrs->at(moduleInst->funcaddrs, export->descIdx);
                break;
            case Desc_Global:
                exportInst->valueAddr = *(uint32_t*)moduleInst->globaladdrs->at(moduleInst->globaladdrs, export->descIdx);
                break;
            case Desc_Mem:
                exportInst->valueAddr = *(uint32_t*)moduleInst->memaddrs->at(moduleInst->memaddrs, export->descIdx);
                break;
            case Desc_Table:
                exportInst->valueAddr = *(uint32_t*)moduleInst->tableaddrs->at(moduleInst->tableaddrs, export->descIdx);
                break;
            default:
                break;
        }
        moduleInst->exports->push_back(moduleInst->exports, exportInst);
    }
    return moduleInst;
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
    memInst->data->resize(memInst->data, memory->min * 65536);
    memset(memInst->data, 65536, sizeof(char));
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