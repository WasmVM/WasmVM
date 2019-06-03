#ifndef WASMVM_DATATYPES_QUEUE__DEF
#define WASMVM_DATATYPES_QUEUE__DEF

#include <stddef.h>
#include <dataTypes/queue_t.h>

typedef struct queue_tNode_ * queue_tNode;

struct queue_tNode_ {
    void* data;
    queue_tNode next;
};

struct queue_t_ {
    queue_tNode head;
    queue_tNode tail;
    size_t size;
    void (*freeElem)(void* elem);
};

#endif