#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f64_convert_s_i32, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i32Value(-23);
    push_Value(stack, value1);

    // Run
    runtime_f64_convert_s_i32(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -23);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
