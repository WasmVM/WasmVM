#ifndef WASMVM_EXECUTOR_DEF
#define WASMVM_EXECUTOR_DEF

#include <stdint.h>
#include <core/Store.h>
#include <dataTypes/vector_p.h>
#include <instance/ModuleInst.h>

typedef struct Executor_ * Executor;

Executor new_Executor();
void free_Executor(Executor executor);

int executor_run(Executor executor);
int executor_stop(Executor executor);
int executor_join(Executor executor);
int executor_addModule(Executor executor, ModuleInst* module, uint32_t startFuncIndex);

vector_p executor_get_modules(Executor executor);
Store executor_get_store(Executor executor);

#endif