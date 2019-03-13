#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_and, regular)
{
    // prepare
    Stack* stack = new_Stack();
    Value *val1 = new_i32Value(5), *val2 = new_i32Value(3);
    stack->entries->push(stack->entries, val1);
    stack->entries->push(stack->entries, val2);

    // run
    runtime_i32_and(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, 1);

    // clean
    free_Value(check);
    free_Stack(stack);
}
