#ifndef WASMVM_EXECUTOR_DEF
#define WASMVM_EXECUTOR_DEF

#include <stdint.h>
#include <core/Store.h>
#include <core/Core.h>
#include <instance/ModuleInst.h>
#include <dataTypes/vector.h>

typedef enum {
    Executor_Stop,
    Executor_Running,
    Executor_Terminated
} ExecutorStatus;

typedef struct Executor_ {
    Store* store;
    vector* modules; // ModuleInst
    vector* cores; // Core
    ExecutorStatus status;
    int (*run)(struct Executor_* executor);
    int (*stop)(struct Executor_* executor);
    int (*addModule)(struct Executor_* executor, ModuleInst* module, uint32_t startFuncIndex);
} Executor;

Executor* new_Executor();
void free_Executor(Executor* executor);

#endif