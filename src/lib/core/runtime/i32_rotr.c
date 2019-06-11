#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

int runtime_i32_rotr(Stack stack)
{
    Value *value1 = NULL;
    Value *value2 = NULL;
    uint32_t overValue = 0;
    uint32_t temp1 = 0;
    uint32_t temp2 = 0;

    pop_Value(stack,&value2);
    pop_Value(stack,&value1);

    temp1 = value1->value.i32;
    temp2 = value2->value.i32;

    overValue = temp1 << (32 - temp2);
    temp1 >>= temp2;
    temp1 |= overValue;

    push_Value(stack, new_i32Value(temp1));

    free(value1);
    free(value2);

    return 0;
}
