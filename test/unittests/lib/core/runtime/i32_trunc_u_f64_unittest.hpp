#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_trunc_u_f64, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f64Value(-22.5));
    runtime_i32_trunc_u_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 22);
    free_Value(check);

    push_Value(stack, new_f64Value(22.5));
    runtime_i32_trunc_u_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 22);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
