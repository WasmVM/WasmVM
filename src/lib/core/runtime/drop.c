#include <core/Runtime.h>
#include <dataTypes/Entry.h>
#include <dataTypes/Value.h>
#include <stddef.h>
#include <stdlib.h>

int runtime_drop(Stack* stack)
{
    Value *value = NULL;
    if(stack->entries->pop(stack->entries, (void**)&value)) return -1;
    free_Value(value);
    return 0;
}
