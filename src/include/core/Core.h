#ifndef WASMVM_CORE_DEF
#define WASMVM_CORE_DEF

#include <stdint.h>
#include <Executor.h>
#include <instance/ModuleInst.h>

typedef struct Core_ * Core;

Core new_Core(Executor executor, ModuleInst* module, uint32_t startFuncAddr);
void clean_Core(Core core);
void free_Core(Core core);

int core_run(Core core);
int core_pause(Core core);
int core_resume(Core core);
int core_stop(Core core);

#endif