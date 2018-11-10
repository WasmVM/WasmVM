#ifndef WASMVM_COMPONENT_DEF
#define WASMVM_COMPONENT_DEF

#include <pthread.h>

typedef struct Component_ {
    pthread_t* thread;
    _Bool isTerminated;
    int (*activate)(struct Component_* thisComponent);
    void (*terminate)(struct Component_* thisComponent);
} Component;

#endif