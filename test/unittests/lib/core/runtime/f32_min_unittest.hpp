#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f32_min, eq)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f32Value(5.0f), *value2 = new_f32Value(5.0f);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f32_min(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 5.0f);

    // Clean
    free_Value(check);
    free_Stack(stack);
}

SKYPAT_F(Runtime_f32_min, lt)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f32Value(-5.0f), *value2 = new_f32Value(3.0f);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f32_min(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -5.0f);

    // Clean
    free_Value(check);
    free_Stack(stack);
}

SKYPAT_F(Runtime_f32_min, gt)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f32Value(5.0f), *value2 = new_f32Value(-3.0f);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f32_min(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -3.0f);

    // Clean
    free_Value(check);
    free_Stack(stack);
}