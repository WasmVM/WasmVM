#ifndef WASMVM_DATATYPES_LIST__DEF
#define WASMVM_DATATYPES_LIST__DEF

#include <stddef.h>
#include <dataTypes/list_p.h>

typedef struct listNode_ listNode;

struct listNode_ {
    void* data;
    struct listNode_* next;
};

struct list_p_ {
    listNode* head;
    listNode* tail;
    size_t size;
    void (*freeElem)(void* elem);
};

#endif