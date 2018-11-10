#ifndef WASMVM_DATATYPES_LIST_DEF
#define WASMVM_DATATYPES_LIST_DEF

#include <stddef.h>

typedef struct listNode_ {
    void* data;
    struct listNode_* next;
} listNode;

typedef struct list_{
    listNode* head;
    listNode* tail;
    size_t size;
    void (*freeElem)(void* elem);
    void (*push_back)(struct list_* thisList, const void* value);
    void* (*at)(struct list_* thisList, size_t index);
    int (*removeAt)(struct list_* thisList, size_t index);
} list;

list* new_list(void (*freeElem)(void* elem));
void free_list(list* thislistPtr);

#endif