#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_wrap_i64, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i64Value(-20);
    push_Value(stack, value1);

    // Run
    runtime_i32_wrap_i64(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, -20);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
