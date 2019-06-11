#include <core/Runtime.h>
#include <stddef.h>
#include <dataTypes/Value.h>

int runtime_i64_clz(Stack stack)
{
    Value *value1 = NULL;
    uint8_t count = 0;
    uint64_t tmp;

    pop_Value(stack,&value1);
    tmp = value1->value.i64;
    free_Value(value1);

    // bit shift
    if(!tmp) {
        count = 64;
    } else {
        if ((tmp >> 32) == 0) {
            count += 32;
            tmp <<= 32;
        }
        if ((tmp >> 48) == 0) {
            count += 16;
            tmp <<= 16;
        }
        if ((tmp >> 56) == 0) {
            count += 8;
            tmp <<= 8;
        }
        if ((tmp >> 60) == 0) {
            count += 4;
            tmp <<= 4;
        }
        if ((tmp >> 62) == 0) {
            count += 2;
            tmp <<= 2;
        }
        if ((tmp >> 63) == 0) {
            count += 1;
        }
    }
    push_Value(stack, new_i64Value(count));

    return 0;
}
