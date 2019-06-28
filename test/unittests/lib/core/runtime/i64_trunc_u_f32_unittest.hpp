#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_trunc_u_f32, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f32Value(-22.5));
    runtime_i64_trunc_u_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 22);
    free_Value(check);

    push_Value(stack, new_f32Value(22.5));
    runtime_i64_trunc_u_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 22);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
