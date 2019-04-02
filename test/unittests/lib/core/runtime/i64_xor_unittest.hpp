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
    Stack* stack = new_Stack();
    Value *val1 = new_i64Value(4L << 32), *val2 = new_i64Value(5L << 32);
    push_Value(stack, val1);
    push_Value(stack, val2);

    // run
    runtime_i64_xor(stack);

    // check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 1L << 32);

    // clean
    free_Value(check);
    free_Stack(stack);
}
