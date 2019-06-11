#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_const, regular)
{
    // Prepare
    Stack stack = new_Stack();

    // Run
    runtime_i32_const(stack, 10);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 10);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
