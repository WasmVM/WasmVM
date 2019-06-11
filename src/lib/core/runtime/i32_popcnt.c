#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stdlib.h>

int runtime_i32_popcnt(Stack stack)
{
    Value *value1 = NULL;
    pop_Value(stack,&value1);

    uint32_t n = value1->value.u32;

    // 11 (2) = 1 * 2 + 1 (10)
    // ab (2) = a * 2 + b (10)
    // ab >> 1 = a
    // ab - (ab >> 1) = a + b
    n -= (n >> 1)  & 0x55555555;
    n  = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n  = ((n >> 4) + n) & 0x0F0F0F0F;
    n += n >> 8;
    n += n >> 16;
    n &= 0x0000003F;
    value1->value.u32 = n;
    push_Value(stack, value1);

    return 0;
}
