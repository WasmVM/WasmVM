#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i64_gt_u(Stack* stack)
{
    Value *value_1 = NULL, *value_2 = NULL;
    stack->entries->pop(stack->entries, (void**)&value_2);
    stack->entries->pop(stack->entries, (void**)&value_1);
    stack->entries->push(stack->entries, new_i64Value(value_1->value.u64 > value_2->value.u64));

    free(value_1);
    free(value_2);
    return 0;
}
