#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i64_lt_s() unittest and check the result of runtime_i64_lt_s().
 */
void i64_lt_s_check(Stack stack, int64_t value_1, int64_t value_2, int64_t expected)
{
    Value *check = NULL;
    Value *_value_1 = new_i64Value(value_1);
    Value *_value_2 = new_i64Value(value_2);

    push_Value(stack, _value_1);
    push_Value(stack, _value_2);
    runtime_i64_lt_s(stack);

    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, expected);
    free_Value(check);
}

SKYPAT_F(Runtime_i64_lt_s, regular)
{
    Stack stack = new_Stack();

    // case 1: 20 < 3
    i64_lt_s_check(stack, 20LL, 3LL, 0);

    // case 2: (-20) < 3
    i64_lt_s_check(stack, -20LL, 3LL, 1);

    // case 3: 20 < (-3)
    i64_lt_s_check(stack, 20LL, -3LL, 0);

    // case 4: (-20) < (-3)
    i64_lt_s_check(stack, -20LL, -3LL, 1);

    // case 5: (-20) < (-20)
    i64_lt_s_check(stack, -20LL, -20LL, 0);

    // clean
    free_Stack(stack);
}
