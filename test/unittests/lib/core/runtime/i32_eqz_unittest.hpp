#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_eqz() unittest and check the result of runtime_i32_eqz().
 */
void i32_eqz_check(Stack stack, int32_t value_1, int32_t expected)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1);

    push_Value(stack, _value_1);
    runtime_i32_eqz(stack);

    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, expected);
    free_Value(check);
}

SKYPAT_F(Runtime_i32_eqz, regular)
{
    Stack stack = new_Stack();

    // case 1: 10
    i32_eqz_check(stack, 10, 0);

    // case 2: -10
    i32_eqz_check(stack, -10, 0);

    // case 3: 0
    i32_eqz_check(stack, 0, 1);

    // clean
    free_Stack(stack);
}
