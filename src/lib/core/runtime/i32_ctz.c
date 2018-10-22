#include <core/Runtime.h>

int runtime_i32_ctz(Stack* stack){
    Value *value1 = NULL;
    int32_t num = 1;
    uint32_t tmp;

    stack->entries->pop(stack->entries, (void**)&value1);
    tmp = value1->value.i32;
    // bit shift
    if (tmp == 0) num = 32;
    if ((tmp << 16) == 0) { num += 16; tmp >>= 16;}
    if ((tmp << 8) == 0) { num += 8; tmp >>= 8;}
    if ((tmp << 4) == 0) { num += 4; tmp >>= 4;}
    if ((tmp << 2) == 0) { num += 2; tmp >>= 2;}
    if ((tmp << 1) == 0) { num += 1;}
    stack->entries->push(stack->entries, new_i32Value(num));

    free(value1);
    return 0;
}
