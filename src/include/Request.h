#ifndef WASMVM_REQUEST_DEF
#define WASMVM_REQUEST_DEF

#include <dataTypes/queue_p.h>

typedef struct Request_ {
    queue_p stages;
    void (*free)(struct Request_* request);
} Request;
#endif