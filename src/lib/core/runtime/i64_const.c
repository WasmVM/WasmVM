#include <core/Runtime.h>
#include <dataTypes/Value.h>

int runtime_i64_const(Stack* stack, int64_t value)
{
    stack->entries->push(stack->entries, new_i64Value(value));
    return 0;
}
