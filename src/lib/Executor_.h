#ifndef WASMVM_EXECUTOR__DEF
#define WASMVM_EXECUTOR__DEF

#include <stdatomic.h>
#include <pthread.h>
#include <core/Store.h>
#include <dataTypes/vector_p.h>

enum ExecutorStatus_ {
    Executor_Stop,
    Executor_Running,
    Executor_Terminated
};

struct Executor_ {
    atomic_int runningCores;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Store store;
    vector_p modules; // ModuleInst
    vector_p cores; // Core
    enum ExecutorStatus_ status;
};

#endif