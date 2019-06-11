#ifndef WASMVM_INSTANCIATOR_DEF
#define WASMVM_INSTANCIATOR_DEF

#include <Executor.h>
#include <structures/WasmModule.h>

typedef struct Instanciator_ * Instanciator;

Instanciator new_Instanciator(WasmModule* module, Executor executor);

#endif