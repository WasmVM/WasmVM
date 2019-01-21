#ifndef WASMVM_CORE_STACK_DEF
#define WASMVM_CORE_STACK_DEF

#include <dataTypes/stack.h>

typedef struct {
    stack* entries; // Entry
} Stack;

Stack* new_Stack(void (*freeElem)(void* elem));
void free_Stack(Stack* thisStack);

#endif
