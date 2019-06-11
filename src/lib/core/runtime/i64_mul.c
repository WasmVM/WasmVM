#include <core/Runtime.h>
#include <stddef.h>
#include <dataTypes/Value.h>

int runtime_i64_mul(Stack stack)
{
    Value *value1 = NULL, *value2 = NULL;
    pop_Value(stack,&value2);
    pop_Value(stack,&value1);
    push_Value(stack, new_i64Value(value1->value.i64 * value2->value.i64));
    free_Value(value1);
    free_Value(value2);
    return 0;
}
