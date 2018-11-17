#include <core/Runtime.h>

#include <dataTypes/Value.h>

int runtime_i32_const(Stack* stack, int32_t value)
{
    stack->entries->push(stack->entries, new_i32Value(value));
    return 0;
}