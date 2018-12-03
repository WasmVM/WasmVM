#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

/*
 * This function applys runtime_i32_eqz() unittest and return the result of runtime_i32_eqz().
 */
int32_t i32_eqz_check(Stack* stack, int32_t value_1)
{
    Value *check = NULL;
    Value *_value_1 = new_i32Value(value_1);

    stack->entries->push(stack->entries, _value_1);
    runtime_i32_eqz(stack);

    stack->entries->pop(stack->entries, (void**)&check);
    return check->value.i32;
}

SKYPAT_F(Runtime_i32_eqz, regular)
{
    Stack* stack = new_Stack();
    int32_t check_value;

    // case 1: 10
    check_value = i32_eqz_check(stack, 10);
    EXPECT_EQ(check_value, 0);

    // case 2: -10
    check_value = i32_eqz_check(stack, -10);
    EXPECT_EQ(check_value, 0);

    // case 3: 0
    check_value = i32_eqz_check(stack, 0);
    EXPECT_EQ(check_value, 1);
}
