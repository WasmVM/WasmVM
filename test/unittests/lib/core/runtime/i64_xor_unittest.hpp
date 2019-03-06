#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_xor, regular)
{
    // prepare
    Stack* stack = new_Stack((void (*)(void*))free_Value);
    Value *val1 = new_i64Value(4L << 32), *val2 = new_i64Value(5L << 32);
    stack->entries->push(stack->entries, val1);
    stack->entries->push(stack->entries, val2);

    // run
    runtime_i64_xor(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 1L << 32);

    // clean
    free_Value(check);
    free_Stack(stack);
}
