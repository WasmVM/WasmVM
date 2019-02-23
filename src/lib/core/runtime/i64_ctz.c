#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <stddef.h>

int runtime_i64_ctz(Stack* stack)
{
    Value *value1 = NULL;
    uint8_t count = 0;
    uint64_t tmp;

    stack->entries->pop(stack->entries, (void**)&value1);
    tmp = value1->value.i64;

    // bit shift
    if (tmp == 0) {
        count = 64;
    } else {
        if ((tmp << 32) == 0) {
            count += 32;
            tmp >>= 32;
        }
        if ((tmp << 48) == 0) {
            count += 16;
            tmp >>= 16;
        }
        if ((tmp << 56) == 0) {
            count += 8;
            tmp >>= 8;
        }
        if ((tmp << 60) == 0) {
            count += 4;
            tmp >>= 4;
        }
        if ((tmp << 62) == 0) {
            count += 2;
            tmp >>= 2;
        }
        if (!tmp) {
            count += 1;
        }
    }
    stack->entries->push(stack->entries, new_i32Value(count));

    free_Value(value1);
    return 0;
}
