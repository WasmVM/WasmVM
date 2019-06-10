#ifndef WASMVM_DATATYPES_STACK__DEF
#define WASMVM_DATATYPES_STACK__DEF

#include <stddef.h>
#include <dataTypes/stack_p.h>

typedef struct stackNode_ * stackNode;

struct stackNode_ {
    void* data;
    stackNode next;
};

struct stack_p_ {
    stackNode head;
    stackNode tail;
    size_t size;
    void (*freeElem)(void* elem);
};

#endif