#include <Instanciator.h>

#include <stdlib.h>
#include <string.h>
#include <instance/ExportInst.h>
#include <instance/ModuleInst.h>

static void setInput(Instanciator* instanciator, void* input)
{
    instanciator->parent.input = input;
}
static void* getOutput(Instanciator* instanciator)
{
    return instanciator->parent.output;
}
static int matchExport(WasmImport* import, vector* moduleInsts, ExportInst** matched)
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
            *matched = export;
            return 0;
        }
    }
    return -2;
}
static int runInstanciator(Instanciator* instanciator)
{
    WasmModule* module = instanciator->parent.input;
    // Match import
    for(size_t i = 0; i < module->imports->length; ++i) {
        WasmImport* import = module->imports->at(module->imports, i);
        ExportInst* matched = NULL;
        int matchResult = matchExport(import, instanciator->moduleInsts, &matched);
        if(matchResult) {
            return matchResult;
        }
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