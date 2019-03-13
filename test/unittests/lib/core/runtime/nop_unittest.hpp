#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_nop, regular)
{
    Stack* stack = new_Stack();
    int32_t check_value;

    /*
     * "insert 3", "nop"
     * "insert -20", "nop"
     * "call 3 / (-20)", "nop"
     */

    Value *value_1 = new_i32Value(3);
    Value *value_2 = new_i32Value(-20);
    Value *check = NULL;

    stack->entries->push(stack->entries, value_1);
    runtime_nop();
    stack->entries->push(stack->entries, value_2);
    runtime_nop();
    runtime_i32_add(stack);
    runtime_nop();
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, -17);

    // clean
    free_Value(check);
    free_Stack(stack);
}
