#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f32_convert_s_i32, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_i64Value(-23));
    runtime_f32_convert_u_i64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 23);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_i64Value(23));
    runtime_f32_convert_u_i64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 23);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
