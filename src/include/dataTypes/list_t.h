#ifndef WASMVM_DATATYPES_LIST_DEF
#define WASMVM_DATATYPES_LIST_DEF

#include <defines.h>

#define list_node_t(T) struct { \
    T data; \
    void* next; \
}

#define list_t(T) struct { \
    unsigned int size; \
    list_node_t(T) *head, *end; \
}

#define free_list(T, L) while(L.head){ \
    L.end = L.head; \
    L.head = (list_node_t(T)*) L.head->next; \
    free_func(L.end) \
}

#endif