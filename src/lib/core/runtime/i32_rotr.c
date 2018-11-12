#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

int runtime_i32_rotr(Stack* stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;
    int32_t overValue = 0;
    int32_t temp1 = 0;
    int32_t temp2 = 0;

    stack->entries->pop(stack->entries, (void **)&value2);
    stack->entries->pop(stack->entries, (void **)&value1);

    temp1 = value1->value.i32;
    temp2 = value2->value.i32;

    overValue = temp1 << (32 - temp2);
    temp1 >>= temp2;
    temp1 |= overValue;

    stack->entries->push(stack->entries, new_i32Value(temp1));

    free(value1);
    free(value2);

    return 0;
}
