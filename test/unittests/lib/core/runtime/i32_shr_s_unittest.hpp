#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_shr_s_positive, regular)
{
    // Prepare
    Stack* stack = new_Stack((void (*)(void*))free_Value);
    Value *value1 = new_i32Value(104), *value2 = new_i32Value(2);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // Run
    runtime_i32_shr_s(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, 26);

    // Clean
    free_Value(check);
    free_Stack(stack);
}

SKYPAT_F(Runtime_i32_shr_s_negative, regular)
{
    // Prepare
    Stack* stack = new_Stack((void (*)(void*))free_Value);
    Value *value1 = new_i32Value(-24), *value2 = new_i32Value(2);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // Run
    runtime_i32_shr_s(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, -6);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
