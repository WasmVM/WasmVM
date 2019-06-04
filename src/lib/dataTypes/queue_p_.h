#ifndef WASMVM_DATATYPES_QUEUE__DEF
#define WASMVM_DATATYPES_QUEUE__DEF

#include <stddef.h>
#include <dataTypes/queue_p.h>

typedef struct queueNode_ * queueNode;

struct queueNode_ {
    void* data;
    queueNode next;
};

struct queue_p_ {
    queueNode head;
    queueNode tail;
    size_t size;
    void (*freeElem)(void* elem);
};

#endif