#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_ge_s, regular)
{
    // prepare
    Stack* stack = new_Stack();
    Value *value1 = new_i64Value(-100L << 32);
    Value *value2 = new_i64Value(10L << 32);
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // run
    runtime_i64_ge_s(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 0);

    // clean
    free_Value(check);
    free_Stack(stack);
}
