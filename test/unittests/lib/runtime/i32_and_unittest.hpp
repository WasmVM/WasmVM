#include <skypat/skypat.h>

#define _Bool bool
extern "C"{
    #include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i32_and, regular)
{
    // Prepare
    Stack* stack = new_Stack();
    Value *val1 = new_i32Value(5), *val2 = new_i32Value(3);
    stack->entries->push(stack->entries, val1);
    stack->entries->push(stack->entries, val2);

    // Run
    runtime_i32_and(stack);

    // Check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i32, 1);
}
