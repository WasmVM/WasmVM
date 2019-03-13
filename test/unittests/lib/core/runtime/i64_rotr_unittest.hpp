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
    Stack* stack = new_Stack();
    Value *value1 = new_i64Value(3), *value2 = new_i64Value(2);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // Run
    runtime_i64_rotr(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 3L << 62);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
