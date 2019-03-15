#ifndef WASMVM_INSTANCIATOR_DEF
#define WASMVM_INSTANCIATOR_DEF

#include <Stage.h>
#include <structures/WasmModule.h>
#include <core/Store.h>
#include <dataTypes/vector.h>

typedef struct {
    Stage parent;
    Store* store;
    vector* moduleInsts;
} Instanciator;

Instanciator* new_Instanciator(WasmModule* module, Store* store, vector* moduleInsts);

#endif