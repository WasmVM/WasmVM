#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_ge_u, regular)
{
    // prepare
    Stack stack = new_Stack();
    Value *value1 = new_i64Value(-100);
    Value *value2 = new_i64Value(10);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // run
    runtime_i64_ge_u(stack);

    // check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 1);

    // clean
    free_Value(check);
    free_Stack(stack);
}
