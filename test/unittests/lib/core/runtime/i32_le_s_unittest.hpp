#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_le_s() unittest and return the result of runtime_i32_le_s().
 */
int32_t i32_le_s_check(Stack* stack, int32_t value_1, int32_t value_2)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1);
    Value *_value_2 = new_i32Value(value_2);

    stack->entries->push(stack->entries, _value_1);
    stack->entries->push(stack->entries, _value_2);
    runtime_i32_le_s(stack);

    stack->entries->pop(stack->entries, (void**)&check);
    return check->value.i32;
}

SKYPAT_F(Runtime_i32_le_s, regular)
{
    Stack* stack = new_Stack();
    int32_t check_value;

    // case 1: 20 <= 3
    check_value = i32_le_s_check(stack, 20, 3);
    EXPECT_EQ(check_value, 0);

    // case 2: (-20) <= 3
    check_value = i32_le_s_check(stack, -20, 3);
    EXPECT_EQ(check_value, 1);

    // case 3: 20 <= (-3)
    check_value = i32_le_s_check(stack, 20, -3);
    EXPECT_EQ(check_value, 0);

    // case 4: (-20) <= (-3)
    check_value = i32_le_s_check(stack, -20, -3);
    EXPECT_EQ(check_value, 1);

    // case 5: (-20) <= (-20)
    check_value = i32_le_s_check(stack, -20, -3);
    EXPECT_EQ(check_value, 1);
}
