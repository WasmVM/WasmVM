#ifndef WASMVM_DATATYPES_LIST__DEF
#define WASMVM_DATATYPES_LIST__DEF

#include <stddef.h>
#include <dataTypes/list_t.h>

typedef struct listNode_ listNode;

struct listNode_ {
    void* data;
    struct listNode_* next;
};

struct list_ {
    listNode* head;
    listNode* tail;
    size_t size;
    void (*freeElem)(void* elem);
};

#endif