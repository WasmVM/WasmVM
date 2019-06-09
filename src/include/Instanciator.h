#ifndef WASMVM_INSTANCIATOR_DEF
#define WASMVM_INSTANCIATOR_DEF

#include <stdint.h>
#include <Stage.h>
#include <structures/WasmModule.h>
#include <instance/ModuleInst.h>
#include <core/Store.h>
#include <Executor.h>
#include <dataTypes/vector_p.h>

typedef struct {
    Stage parent;
    Executor* executor;
} Instanciator;

Instanciator* new_Instanciator(WasmModule* module, Executor* executor);

#endif