#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include "../../../util/StackHelper.h"
}
#undef _Bool

SKYPAT_F(Runtime_nop, regular)
{
    Stack stack = new_Stack();
    int32_t check_value;

    Value *value_1 = new_i32Value(5);
    Value *value_2 = new_i32Value(4);
    Value *value_3 = new_i32Value(3);
    Value *value_4 = new_i32Value(2);
    Value *value_5 = new_i32Value(1);
    Value *value_6 = new_i32Value(0);

    Value *check1 = NULL;
    Value *check2 = NULL;

    push_Value(stack, value_1);
    push_Value(stack, value_2);
    push_Value(stack, value_3);
    push_Value(stack, value_4);
    push_Value(stack, value_5);
    push_Value(stack, value_6);

    // 5 4 3 2 1 0
    check_value = runtime_select(stack);
    EXPECT_EQ(check_value,0);
    EXPECT_EQ(stack_get_size(stack), 4);

    // 5 4 3 1
    pop_Value(stack,&check1);
    EXPECT_EQ(check1->value.i32, 1);

    // 5 4 3
    check_value = runtime_select(stack);
    EXPECT_EQ(check_value,0);
    EXPECT_EQ(stack_get_size(stack), 1);

    // 5
    pop_Value(stack,&check2);
    EXPECT_EQ(check2->value.i32, 5);

    // empty
    check_value = runtime_select(stack);
    EXPECT_EQ(check_value, -2);

    // clean
    free_Value(check1);
    free_Value(check2);
    free_Stack(stack);
}
