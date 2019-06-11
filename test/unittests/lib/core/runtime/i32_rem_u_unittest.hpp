#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_rem_u() unittest.
 * Check the result of runtime_i32_rem_u().
 */
void i32_rem_u_check(Stack stack, int32_t value_1, int32_t value_2, uint32_t expected)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1); // dividend
    Value *_value_2 = new_i32Value(value_2); // divisor

    push_Value(stack, _value_1);
    push_Value(stack, _value_2);
    runtime_i32_rem_u(stack);

    pop_Value(stack,&check);
    EXPECT_EQ(check->value.u32, expected);
    free_Value(check);
}

SKYPAT_F(Runtime_i32_div_u, regular)
{
    Stack stack = new_Stack();

    // case 1: 20 % 3
    i32_rem_u_check(stack, 20, 3, 2);

    // case 2: (-20) % 3
    i32_rem_u_check(stack, -20, 3, 2);

    // case 3: 20 % (-3)
    i32_rem_u_check(stack, 20, -3, 20);

    // case 4: 15 % 1
    i32_rem_u_check(stack, 15, 1, 0);

    // clean
    free_Stack(stack);
}
