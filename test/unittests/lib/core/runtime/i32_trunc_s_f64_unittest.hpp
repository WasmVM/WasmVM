#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_trunc_s_f64, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f64Value(-22.5);
    push_Value(stack, value1);

    // Run
    runtime_i32_trunc_s_f64(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, -22);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
