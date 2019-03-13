#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_clz, regular)
{
    // prepare
    Stack* stack = new_Stack();
    Value *value1 = new_i64Value(1L << 32);
    stack->entries->push(stack->entries, value1);

    // run
    runtime_i64_clz(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, 31);

    // clean
    free_Value(check);
    free_Stack(stack);
}
