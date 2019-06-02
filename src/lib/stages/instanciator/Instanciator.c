#include <Instanciator.h>

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

static void setInput(Instanciator* instanciator, void* input)
{
    instanciator->parent.input = input;
}
static void* getOutput(Instanciator* instanciator)
{
    return instanciator->parent.output;
}
static int matchFunc(FuncType importType, FuncType exportType)
{
    if(importType->params->length != exportType->params->length) {
        return -1;
    }
    for(size_t i = 0; i < importType->params->length; ++i) {
        if(*(ValueType*)importType->params->at(importType->params, i) != *(ValueType*)exportType->params->at(exportType->params, i)) {
            return -2;
        }
    }
    if(importType->results->length != exportType->results->length) {
        return -3;
    }
    for(size_t i = 0; i < importType->results->length; ++i) {
        if(*(ValueType*)importType->results->at(importType->results, i) != *(ValueType*)exportType->results->at(exportType->results, i)) {
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
    if((memInst->data->length >> 16) < import->desc.limits.min) {
        return -1;
    }
    if(import->desc.limits.max != 0 && (memInst->max == 0 || memInst->max > import->desc.limits.max)) {
        return -2;
    }
    return 0;
}
static int matchTable(WasmImport* import, TableInst* tableInst)
{
    if(tableInst->elem->length < import->desc.limits.min) {
        return -1;
    }
    if(import->desc.limits.max != 0 && (tableInst->max == 0 || tableInst->max > import->desc.limits.max)) {
        return -2;
    }
    return 0;
}
static int matchExport(WasmModule* module, WasmImport* import, vector* moduleInsts, Store* store, ExportInst** matched)
{
    ModuleInst* moduleInst = NULL;
    for(size_t i = 0; i < moduleInsts->length; ++i) {
        ModuleInst* curInst = moduleInsts->at(moduleInsts, i);
        if(!strcmp(curInst->name, import->module)) {
            moduleInst = curInst;
            break;
        }
    }
    if(!moduleInst) {
        *matched = NULL;
        return -1;
    }
    for(size_t i = 0; i < moduleInst->exports->length; ++i) {
        ExportInst* export = moduleInst->exports->at(moduleInst->exports, i);
        if(!strcmp(export->name, import->name)) {
            switch (import->descType) {
                case Desc_Func:
                    if(matchFunc(
                                (FuncType)module->types->at(module->types, import->desc.typeidx),
                                ((FuncInst*)store->funcs->at(store->funcs, export->valueAddr))->type
                            )) {
                        return -2;
                    }
                    break;
                case Desc_Global:
                    if(matchGlobal(import, (GlobalInst*)store->globals->at(store->globals, export->valueAddr))) {
                        return -3;
                    }
                    break;
                case Desc_Mem:
                    if(matchMemory(import, (MemInst*)store->mems->at(store->mems, export->valueAddr))) {
                        return -4;
                    }
                    break;
                case Desc_Table:
                    if(matchTable(import, (TableInst*)store->tables->at(store->tables, export->valueAddr))) {
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
static int runInstanciator(Instanciator* instanciator)
{
    WasmModule* module = instanciator->parent.input;
    ExportInst** exportInsts = NULL;
    if(module->imports->length > 0) {
        exportInsts = (ExportInst**) malloc(sizeof(ExportInst*) * module->imports->length);
        // Match import with exports
        for(size_t i = 0; i < module->imports->length; ++i) {
            WasmImport* import = module->imports->at(module->imports, i);
            ExportInst* matched = NULL;
            int matchResult = matchExport(module, import, instanciator->executor->modules, instanciator->executor->store, &matched);
            if(matchResult) {
                return matchResult;
            }
            exportInsts[i] = matched;
        }
    }
    // Allocate moduleInst
    ModuleInst* moduleInst = allocate_Module(module, instanciator->executor->store, exportInsts, module->imports->length);
    free(exportInsts);
    // Elems
    for(size_t i = 0; i < module->elems->length; ++i) {
        WasmElem* elem = (WasmElem*)module->elems->at(module->elems, i);
        TableInst* tableInst = (TableInst*)instanciator->executor->store->tables->at(
                                   instanciator->executor->store->tables,
                                   *(uint32_t*)moduleInst->tableaddrs->at(
                                       moduleInst->tableaddrs,
                                       elem->table
                                   )
                               );
        if(elem->offset.value.i32 + elem->init->length > tableInst->elem->length) {
            return -8;
        }
        for(size_t j = 0; j < elem->init->length; ++j) {
            uint32_t* address = (uint32_t*)moduleInst->funcaddrs->at(moduleInst->funcaddrs, *(uint32_t*)elem->init->at(elem->init, j));
            uint32_t* element = (uint32_t*)tableInst->elem->at(tableInst->elem, elem->offset.value.i32 + j);
            *element = *address;
        }
    }
    // Datas
    for(size_t i = 0; i < module->datas->length; ++i) {
        WasmData* data = (WasmData*)module->datas->at(module->datas, i);
        MemInst* memInst = (MemInst*)instanciator->executor->store->mems->at(
                               instanciator->executor->store->mems,
                               *(uint32_t*)moduleInst->memaddrs->at(
                                   moduleInst->memaddrs,
                                   data->data
                               )
                           );
        if(data->offset.value.i32 + data->init->length > memInst->data->length) {
            return -9;
        }
        memcpy(((char*)memInst->data->data) + data->offset.value.i32, data->init->data, sizeof(char) * data->init->length);
    }
    // Start
    instanciator->executor->addModule(instanciator->executor, moduleInst, module->start);
    // Free WasmModule
    free_WasmModule(module);
    return 0;
}

Instanciator* new_Instanciator(WasmModule* module, Executor* executor)
{
    Instanciator* instanciator = (Instanciator*) malloc(sizeof(Instanciator));
    instanciator->parent.input = module;
    instanciator->parent.output = NULL;
    instanciator->parent.setInput = (void(*)(Stage*, void*))setInput;
    instanciator->parent.getOutput = (void*(*)(Stage*))getOutput;
    instanciator->parent.run = (int(*)(Stage*))runInstanciator;
    instanciator->executor = executor;
    return instanciator;
}