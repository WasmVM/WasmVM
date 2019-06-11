#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i64_rotl(Stack stack)
{
    Value *value1 = NULL, *value2 = NULL;
    uint64_t tmp;

    pop_Value(stack,&value2);
    pop_Value(stack,&value1);

    tmp = (value1->value.u64 << value2->value.i64) | (value1->value.u64 >> (64 - value2->value.i64));

    push_Value(stack, new_i64Value(tmp));

    free(value1);
    free(value2);
    return 0;
}
