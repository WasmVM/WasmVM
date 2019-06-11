#include "Instanciator_.h"

#include <stdlib.h>
#include <string.h>
#include <core/Store.h>
#include <structures/WasmModule.h>
#include <structures/WasmElem.h>
#include <structures/WasmData.h>
#include <instance/ExportInst.h>
#include <instance/FuncInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>
#include <instance/TableInst.h>
#include "Allocates.h"

static int matchFunc(FuncType importType, FuncType exportType)
{
    if(vector_size(importType->params) != vector_size(exportType->params)) {
        return -1;
    }
    for(size_t i = 0; i < vector_size(importType->params); ++i) {
        if(*vector_at(ValueType*, importType->params, i) != *vector_at(ValueType*, exportType->params, i)) {
            return -2;
        }
    }
    if(vector_size(importType->results) != vector_size(exportType->results)) {
        return -3;
    }
    for(size_t i = 0; i < vector_size(importType->results); ++i) {
        if(*vector_at(ValueType*, importType->results, i) != *vector_at(ValueType*, exportType->results, i)) {
            return -4;
        }
    }
    return 0;
}
static int matchGlobal(WasmImport* import, GlobalInst* globalInst)
{
    if(import->desc.global.mut != globalInst->mut) {
        return -1;
    }
    if(import->desc.global.valueType != globalInst->value.type) {
        return -2;
    }
    return 0;
}
static int matchMemory(WasmImport* import, MemInst* memInst)
{
    if((vector_size(memInst->data) >> 16) < import->desc.limits.min) {
        return -1;
    }
    if(import->desc.limits.max != 0 && (memInst->max == 0 || memInst->max > import->desc.limits.max)) {
        return -2;
    }
    return 0;
}
static int matchTable(WasmImport* import, TableInst* tableInst)
{
    if(vector_size(tableInst->elem) < import->desc.limits.min) {
        return -1;
    }
    if(import->desc.limits.max != 0 && (tableInst->max == 0 || tableInst->max > import->desc.limits.max)) {
        return -2;
    }
    return 0;
}
static int matchExport(WasmModule* module, WasmImport* import, vector_p moduleInsts, Store store, ExportInst** matched)
{
    ModuleInst* moduleInst = NULL;
    for(size_t i = 0; i < vector_size(moduleInsts); ++i) {
        ModuleInst* curInst = vector_at(ModuleInst*, moduleInsts, i);
        if(!strcmp(curInst->name, import->module)) {
            moduleInst = curInst;
            break;
        }
    }
    if(!moduleInst) {
        *matched = NULL;
        return -1;
    }
    for(size_t i = 0; i < vector_size(moduleInst->exports); ++i) {
        ExportInst* export = vector_at(ExportInst*, moduleInst->exports, i);
        if(!strcmp(export->name, import->name)) {
            switch (import->descType) {
                case Desc_Func:
                    if(matchFunc(vector_at(FuncType, module->types, import->desc.typeidx), (vector_at(FuncInst*, store->funcs, export->valueAddr))->type)) {
                        return -2;
                    }
                    break;
                case Desc_Global:
                    if(matchGlobal(import, vector_at(GlobalInst*, store->globals, export->valueAddr))) {
                        return -3;
                    }
                    break;
                case Desc_Mem:
                    if(matchMemory(import, vector_at(MemInst*, store->mems, export->valueAddr))) {
                        return -4;
                    }
                    break;
                case Desc_Table:
                    if(matchTable(import, vector_at(TableInst*, store->tables, export->valueAddr))) {
                        return -5;
                    }
                    break;
                default:
                    return -6;
                    break;
            }
            *matched = export;
            return 0;
        }
    }
    return -7;
}
static int runInstanciator(Instanciator instanciator)
{
    WasmModule* module = instanciator->parent.input;
    ExportInst** exportInsts = NULL;
    if(vector_size(module->imports) > 0) {
        exportInsts = (ExportInst**) malloc(sizeof(ExportInst*) * vector_size(module->imports));
        // Match import with exports
        for(size_t i = 0; i < vector_size(module->imports); ++i) {
            WasmImport* import = vector_at(WasmImport*, module->imports, i);
            ExportInst* matched = NULL;
            int matchResult = matchExport(module, import, executor_get_modules(instanciator->executor), executor_get_store(instanciator->executor), &matched);
            if(matchResult) {
                return matchResult;
            }
            exportInsts[i] = matched;
        }
    }
    // Allocate moduleInst
    ModuleInst* moduleInst = allocate_Module(module, executor_get_store(instanciator->executor), exportInsts, vector_size(module->imports));
    free(exportInsts);
    // Elems
    for(size_t i = 0; i < vector_size(module->elems); ++i) {
        WasmElem* elem = vector_at(WasmElem*, module->elems, i);
        TableInst* tableInst = vector_at(TableInst*, executor_get_store(instanciator->executor)->tables, *vector_at(uint32_t*, moduleInst->tableaddrs, elem->table));
        if(elem->offset.value.i32 + vector_size(elem->init) > vector_size(tableInst->elem)) {
            return -8;
        }
        for(size_t j = 0; j < vector_size(elem->init); ++j) {
            uint32_t* address = vector_at(uint32_t*, moduleInst->funcaddrs, *vector_at(uint32_t*, elem->init, j));
            uint32_t* element = vector_at(uint32_t*, tableInst->elem, elem->offset.value.i32 + j);
            *element = *address;
        }
    }
    // Datas
    for(size_t i = 0; i < vector_size(module->datas); ++i) {
        WasmData* data = vector_at(WasmData*, module->datas, i);
        MemInst* memInst = vector_at(MemInst*, executor_get_store(instanciator->executor)->mems, *vector_at(uint32_t*, moduleInst->memaddrs, data->data));
        if(data->offset.value.i32 + vector_size(data->init) > vector_size(memInst->data)) {
            return -9;
        }
        memcpy(vector_data(char*, memInst->data) + data->offset.value.i32, vector_data(void*, data->init), sizeof(char) * vector_size(data->init));
    }
    // Start
    executor_addModule(instanciator->executor, moduleInst, module->start);
    // Free WasmModule
    free_WasmModule(module);
    return 0;
}

Instanciator new_Instanciator(WasmModule* module, Executor executor)
{
    Instanciator instanciator = (Instanciator) malloc(sizeof(struct Instanciator_));
    instanciator->parent.input = module;
    instanciator->parent.output = NULL;
    instanciator->parent.run = (int(*)(Stage*))runInstanciator;
    instanciator->executor = executor;
    return instanciator;
}