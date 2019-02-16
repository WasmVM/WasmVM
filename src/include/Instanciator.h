#ifndef WASMVM_INSTANCIATOR_DEF
#define WASMVM_INSTANCIATOR_DEF

#include <Stage.h>
#include <WasmModule.h>
#include <core/Store.h>
#include <instance/ModuleInst.h>

typedef struct {
    Stage parent;
    Store* store;
    ModuleInst* moduleInst;
} Instanciator;

Instanciator* new_Instanciator(WasmModule* module, Store* store, ModuleInst* moduleInst);

#endif