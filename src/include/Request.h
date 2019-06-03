#ifndef WASMVM_REQUEST_DEF
#define WASMVM_REQUEST_DEF

#include <dataTypes/queue_t.h>

typedef struct Request_ {
    queue_t stages;
    void (*free)(struct Request_* request);
} Request;
#endif