#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_rem_s() unittest.
 * Return the result of runtime_i32_rem_s().
 */
int32_t i32_rem_s_check(Stack* stack, int32_t value_1, int32_t value_2)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1); // dividend
    Value *_value_2 = new_i32Value(value_2); // divisor

    stack->entries->push(stack->entries, _value_1);
    stack->entries->push(stack->entries, _value_2);
    runtime_i32_rem_s(stack);

    stack->entries->pop(stack->entries, (void**)&check);
    return check->value.u32;
}

SKYPAT_F(Runtime_i32_rem_s, regular)
{
    Stack* stack = new_Stack();
    uint32_t check_value;

    // case 1: 20 % 3
    check_value = i32_rem_s_check(stack, 20, 3);
    EXPECT_EQ(check_value, 2);

    // case 2: (-20) % 3
    check_value = i32_rem_s_check(stack, -20, 3);
    EXPECT_EQ(check_value, -2);

    // case 3: 20 % (-3)
    check_value = i32_rem_s_check(stack, 20, -3);
    EXPECT_EQ(check_value, 2);

    // case 4: 15 % 1
    check_value = i32_rem_s_check(stack, 15, 1);
    EXPECT_EQ(check_value, 0);
}
