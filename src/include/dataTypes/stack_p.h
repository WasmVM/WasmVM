#ifndef WASMVM_DATATYPES_STACK_DEF
#define WASMVM_DATATYPES_STACK_DEF

#include <stddef.h>

typedef struct stack_p_ * stack_p;
typedef struct stackNode_ * stack_iterator;

void free_stack_p(stack_p thisstack);
size_t stack_size(stack_p thisstack);
stack_iterator stack_head(stack_p thisstack);
stack_iterator stack_iterator_next(stack_iterator it);

stack_p new_stack_p_(void (*freeElem)(void* elem));
void stack_push_(stack_p thisstack, void* valuePtr);
void* stack_pop_(stack_p thisstack);
void* stack_top_(stack_p thisstack);
void* stack_iterator_get_(stack_iterator it);

#define new_stack_p(freeElemFunc) new_stack_p_((void(*)(void*))freeElemFunc)
#define stack_push(thisstack, valuePtr) stack_push_(thisstack, (void*) valuePtr)
#define stack_pop(T, thisstack) (T)stack_pop_(thisstack)
#define stack_top(T, thisstack) (T)stack_top_(thisstack)
#define stack_iterator_get(T, it) (T)stack_iterator_get_(it)

#endif
