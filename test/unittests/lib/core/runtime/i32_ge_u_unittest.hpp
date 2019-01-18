#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_ge_u, regular)
{
    // prepare
    Stack* stack = new_Stack();
    Value *value1 = new_i32Value(-100);
    Value *value2 = new_i32Value(10);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // run
    runtime_i32_ge_u(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, 1);

    // clean
    free_Value(check);
    free_Stack(stack);
}
