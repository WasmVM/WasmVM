#include <core/Stack.h>

#include <stdlib.h>
#include <dataTypes/Entry.h>

Stack* new_Stack()
{
    Stack* newStack = (Stack*) malloc(sizeof(Stack));
    newStack->entries = new_stack();
    return newStack;
}
void free_Stack(Stack* thisStack)
{
    free_stack(thisStack->entries);
    free(thisStack);
}