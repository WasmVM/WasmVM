#include <core/Runtime.h>
#include <stddef.h>
#include <dataTypes/Value.h>

int runtime_i64_mul(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2);
    stack->entries->pop(stack->entries, (void**)&value1);
    stack->entries->push(stack->entries, new_i64Value(value1->value.i64 * value2->value.i64));
    free_Value(value1);
    free_Value(value2);
    return 0;
}
