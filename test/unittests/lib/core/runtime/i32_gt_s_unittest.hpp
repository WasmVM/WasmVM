#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_gt_s() unittest and check the result of runtime_i32_gt_s().
 */
void i32_gt_s_check(Stack stack, int32_t value_1, int32_t value_2, int32_t expected)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1);
    Value *_value_2 = new_i32Value(value_2);

    push_Value(stack, _value_1);
    push_Value(stack, _value_2);
    runtime_i32_gt_s(stack);

    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i32, expected);
    free_Value(check);
}

SKYPAT_F(Runtime_i32_gt_s, regular)
{
    Stack stack = new_Stack();

    // case 1: 20 > 3
    i32_gt_s_check(stack, 20, 3, 1);

    // case 2: (-20) > 3
    i32_gt_s_check(stack, -20, 3, 0);

    // case 3: 20 > (-3)
    i32_gt_s_check(stack, 20, -3, 1);

    // case 4: (-20) > (-3)
    i32_gt_s_check(stack, -20, -3, 0);

    // clean
    free_Stack(stack);
}
