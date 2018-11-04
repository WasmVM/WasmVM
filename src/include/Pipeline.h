#ifndef WASMVM_PIPELINE_DEF
#define WASMVM_PIPELINE_DEF

#include <dataTypes/queue.h>

typedef struct Pipeline_{
    queue* requests; // Request*
    void (*addRequest)(struct Pipeline_* pipeline, Request* request);
} Pipeline;

#endif