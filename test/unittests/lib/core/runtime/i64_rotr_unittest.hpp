#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_rotl, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i64Value(3), *value2 = new_i64Value(2);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_i64_rotr(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 3L << 62);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
