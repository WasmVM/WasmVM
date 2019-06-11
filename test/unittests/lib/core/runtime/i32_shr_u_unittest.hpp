#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_shr_u, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i32Value(0x90240059), *value2 = new_i32Value(2);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_i32_shr_u(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, 0x24090016);

    // clean
    free_Value(check);
    free_Stack(stack);
}
