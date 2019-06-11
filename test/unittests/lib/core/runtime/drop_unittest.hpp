#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_drop, regular)
{
    Stack stack = new_Stack();
    int32_t check_value;

    // stack is not empty
    Value *value_1 = new_i32Value(3);
    push_Value(stack, value_1);
    check_value = runtime_drop(stack);
    EXPECT_EQ(check_value, 0);

    // stack is empty
    check_value = runtime_drop(stack);
    EXPECT_EQ(check_value, -1);

    // clean
    free_Stack(stack);
}
