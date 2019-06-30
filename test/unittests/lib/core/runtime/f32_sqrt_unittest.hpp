
#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f32_sqrt, positive)
{
    // prepare
    Stack stack = new_Stack();
    Value *val1 = new_f32Value(0.49f);
    push_Value(stack, val1);

    // run
    runtime_f32_sqrt(stack);

    // check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 0.7f);

    // clean
    free_Value(check);
    free_Stack(stack);
}