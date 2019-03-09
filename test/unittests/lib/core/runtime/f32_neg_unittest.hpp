#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f32_neg, regular)
{
    // prepare
    Stack* stack = new_Stack((void (*)(void*))free_Value);
    Value *val1 = new_f32Value(5), *val2 = new_f32Value(-3);
    stack->entries->push(stack->entries, val1);
    stack->entries->push(stack->entries, val2);

    // run
    runtime_f32_neg(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.f32, 3);

    // clean
    free_Value(check);
    free_Stack(stack);
}
