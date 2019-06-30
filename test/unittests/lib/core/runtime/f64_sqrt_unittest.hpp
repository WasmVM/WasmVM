
#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f64_sqrt, positive)
{
    // prepare
    Stack stack = new_Stack();
    Value *val1 = new_f64Value(0.49);
    push_Value(stack, val1);

    // run
    runtime_f64_sqrt(stack);

    // check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 0.7);

    // clean
    free_Value(check);
    free_Stack(stack);
}