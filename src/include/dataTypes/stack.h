#ifndef WASMVM_STACK_DEF
#define WASMVM_STACK_DEF

#include <stdlib.h>

typedef struct stackNode_ {
    void* data;
    struct stackNode_* next;
} stackNode;

typedef struct stack_{
    stackNode* head;
    unsigned int size;
    void (*push)(struct stack_* thisStack, void* data);
    // If success return 0, or return -1
    int (*pop)(struct stack_* thisStack, void** dataPtr);
    int (*top)(struct stack_* thisStack, void** dataPtr);
} stack;

stack* new_stack();
void free_stack(stack* thisstackPtr);

#endif