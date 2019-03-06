#include <skypat/skypat.h>


#define _Bool bool
extern "C" {
#include <core/Runtime.h>
#include <dataTypes/Value.h>
}
#undef _Bool

SKYPAT_F(Runtime_i64_div_s, regular)
{
    // prepare
    Stack* stack = new_Stack((void (*)(void*))free_Value);
    Value *value1 = new_i64Value(21L << 32); //Dividend
    Value *value2 = new_i64Value(-3L); //Divisor
    stack->entries->push(stack->entries, value1);
    stack->entries->push(stack->entries, value2);

    // run
    runtime_i64_div_s(stack);

    // check
    Value *check = NULL;
    stack->entries->pop(stack->entries, (void**)&check);
    EXPECT_EQ(check->value.i64, -7L << 32);

    // clean
    free_Value(check);
    free_Stack(stack);
}
