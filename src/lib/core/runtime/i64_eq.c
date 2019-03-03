#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i64_eq(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value2);
    stack->entries->pop(stack->entries, (void**)&value1);
    stack->entries->push(stack->entries, new_i64Value(value1->value.i64 == value2->value.i64));
    free(value1);
    free(value2);
    return 0;
}
