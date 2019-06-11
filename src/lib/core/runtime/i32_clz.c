#include <core/Runtime.h>

#include <stdio.h>
#include <stdlib.h>
#include <dataTypes/Value.h>

int runtime_i32_clz(Stack stack)
{
    Value *value1 = NULL;
    int32_t num = 0;
    uint32_t tmp;

    pop_Value(stack,&value1);
    tmp = value1->value.i32;

    // bit shift
    if(tmp == 0) {
        num = 32;
    } else {
        if ((tmp >> 16) == 0) {
            num += 16;
            tmp <<= 16;
        }
        if ((tmp >> 24) == 0) {
            num += 8;
            tmp <<= 8;
        }
        if ((tmp >> 28) == 0) {
            num += 4;
            tmp <<= 4;
        }
        if ((tmp >> 30) == 0) {
            num += 2;
            tmp <<= 2;
        }
        if ((tmp >> 31) == 0) {
            num += 1;
        }
    }
    push_Value(stack, new_i32Value(num));

    free(value1);

    return 0;
}
