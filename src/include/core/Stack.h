#ifndef WASMVM_CORE_STACK_DEF
#define WASMVM_CORE_STACK_DEF

#include <dataTypes/stack.h>
#include <dataTypes/Label.h>

typedef struct {
    stack* entries; // Entry
    uint32_t* curLabel; // current Label
} Stack;

Stack* new_Stack(void (*freeElem)(void* elem));
void free_Stack(Stack* thisStack);
void push_label(Stack* thisStack, Label* label);

#endif
