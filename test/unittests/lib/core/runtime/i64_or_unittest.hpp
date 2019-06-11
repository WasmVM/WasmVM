#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_or, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_i64Value(12LL), *value2 = new_i64Value(10LL);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_i64_or(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 14LL);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
