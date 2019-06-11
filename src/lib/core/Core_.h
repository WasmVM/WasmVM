#ifndef WASMVM_CORE__DEF
#define WASMVM_CORE__DEF

#include <core/Core.h>
#include <pthread.h>
#include <core/Stack.h>

typedef enum {
    Core_Stop,
    Core_Running,
    Core_Paused,
} CoreStatus;

struct Core_ {
    CoreStatus status;
    pthread_t thread;
    Executor executor;
    Stack stack; // Core's private Stack
    ModuleInst* module;
    uint32_t startFuncAddr;
};

#endif