#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i64_rotl(Stack* stack)
{
    Value *value1 = NULL, *value2 = NULL;
    uint64_t tmp;

    stack->entries->pop(stack->entries, (void**)&value2);
    stack->entries->pop(stack->entries, (void**)&value1);

    tmp = (value1->value.u64 << value2->value.i64) | (value1->value.u64 >> (64 - value2->value.i64));

    stack->entries->push(stack->entries, new_i64Value(tmp));

    free(value1);
    free(value2);
    return 0;
}
