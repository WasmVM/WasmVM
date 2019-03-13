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
    Stack* stack = new_Stack();
    Value *value1 = new_i64Value(12L << 32), *value2 = new_i64Value(10L << 32);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // Run
    runtime_i64_or(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 14L << 32);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
