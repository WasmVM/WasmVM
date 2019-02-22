#include <Instanciator.h>

#include <stdlib.h>
#include <string.h>
#include <core/Store.h>
#include <instance/ExportInst.h>
#include <instance/FuncInst.h>
#include <instance/MemInst.h>
#include <instance/ModuleInst.h>
#include <instance/GlobalInst.h>

static void setInput(Instanciator* instanciator, void* input)
{
    instanciator->parent.input = input;
}
static void* getOutput(Instanciator* instanciator)
{
    return instanciator->parent.output;
}
static int matchFunc(FuncType* importType, FuncType* exportType)
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
                                (FuncType*)module->types->at(module->types, import->desc.typeidx),
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
    ExportInst* exportInsts[module->imports->length];
    // Match import with exports
    for(size_t i = 0; i < module->imports->length; ++i) {
        WasmImport* import = module->imports->at(module->imports, i);
        ExportInst* matched = NULL;
        int matchResult = matchExport(module, import, instanciator->moduleInsts, instanciator->store, &matched);
        if(matchResult) {
            return matchResult;
        }
        exportInsts[i] = matched;
    }
    // TODO: Allocate ModuleInst
    return 0;
}

Instanciator* new_Instanciator(WasmModule* module, Store* store, vector* moduleInsts)
{
    Instanciator* instanciator = (Instanciator*) malloc(sizeof(Instanciator));
    instanciator->parent.input = module;
    instanciator->parent.output = NULL;
    instanciator->parent.setInput = (void(*)(Stage*, void*))setInput;
    instanciator->parent.getOutput = (void*(*)(Stage*))getOutput;
    instanciator->parent.run = (int(*)(Stage*))runInstanciator;
    instanciator->moduleInsts = moduleInsts;
    instanciator->store = store;
    return instanciator;
}