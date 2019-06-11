#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_xor, regular)
{
    // prepare
    Stack stack = new_Stack();
    Value *val1 = new_i32Value(4), *val2 = new_i32Value(5);
    push_Value(stack, val1);
    push_Value(stack, val2);

    // run
    runtime_i32_xor(stack);

    // check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 1);

    // clean
    free_Value(check);
    free_Stack(stack);
}
