#ifndef WASMVM_EXECUTOR_DEF
#define WASMVM_EXECUTOR_DEF

#include <Component.h>
#include <core/Store.h>
#include <dataTypes/vector.h>

typedef struct Executor_ {
    Component parent;
    Store* store;
    vector* modules; // ModuleInst
    vector* cores; // Core FIXME:
} Executor;

Executor* new_Executor();
void free_Executor(Executor* executor);

#endif