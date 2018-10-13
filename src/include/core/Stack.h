#ifndef WASMVM_CORE_STACK_DEF
#define WASMVM_CORE_STACK_DEF

#include <stdlib.h>
#include <dataTypes/stack.h>
#include <dataTypes/Entry.h>

typedef struct {
    stack* entries; // Entry
} Stack;

Stack* new_Stack();
void free_Stack(Stack* thisStack);

#endif