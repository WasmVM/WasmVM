#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_shr_u, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i64Value(1L << 63), *value2 = new_i64Value(32);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_i64_shr_u(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 1L << 31);

    // clean
    free_Value(check);
    free_Stack(stack);
}
