#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_add, regular)
{
    // Prepare
    Stack* stack = new_Stack();
    Value *value1 = new_i64Value(20000000000000000), *value2 = new_i64Value(20000000000000010);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // Run
    runtime_i64_add(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 40000000000000010);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
