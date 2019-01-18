#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_le_u() unittest and check the result of runtime_i32_le_u().
 */
void i32_le_u_check(Stack* stack, int32_t value_1, int32_t value_2, int32_t expected)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1);
    Value *_value_2 = new_i32Value(value_2);

    stack->entries->push(stack->entries, _value_1);
    stack->entries->push(stack->entries, _value_2);
    runtime_i32_le_u(stack);

    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, expected);
    free_Value(check);
}

SKYPAT_F(Runtime_i32_le_u, regular)
{
    Stack* stack = new_Stack();

    // case 1: 20 <= 3
    i32_le_u_check(stack, 20, 3, 0);

    // case 2: (-20) <= 3
    i32_le_u_check(stack, -20, 3, 0);

    // case 3: 20 <= (-3)
    i32_le_u_check(stack, 20, -3, 1);

    // case 4: (-20) <= (-3)
    i32_le_u_check(stack, -20, -3, 1);

    // case 5: (-20) <= (-20)
    i32_le_u_check(stack, -20, -3, 1);

    // clean
    free_Stack(stack);
}
