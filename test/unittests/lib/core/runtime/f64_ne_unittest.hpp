#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f64_ne, eq)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f64Value(5.0), *value2 = new_f64Value(5.0);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f64_ne(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 0.0);

    // Clean
    free_Value(check);
    free_Stack(stack);
}

SKYPAT_F(Runtime_f64_ne, lt)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f64Value(-5.0), *value2 = new_f64Value(3.0);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f64_ne(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 1.0);

    // Clean
    free_Value(check);
    free_Stack(stack);
}

SKYPAT_F(Runtime_f64_ne, gt)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f64Value(5.0), *value2 = new_f64Value(-3.0);
    push_Value(stack, value1);
    push_Value(stack, value2);

    // Run
    runtime_f64_ne(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 1.0);

    // Clean
    free_Value(check);
    free_Stack(stack);
}