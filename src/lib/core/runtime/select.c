#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_select(Stack* stack)
{
    Value *value1, *value2, *value3;
    int err = 0;
    err |= stack->entries->pop(stack->entries, (void**)&value3);
    err |= stack->entries->pop(stack->entries, (void**)&value2);
    err |= stack->entries->pop(stack->entries, (void**)&value1);
    if(err) return -1;
    if(value3->value.i32)
        stack->entries->push(stack->entries, value1);
    else
        stack->entries->push(stack->entries, value2);
    return 0;
}
