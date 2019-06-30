#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
}
#undef _Bool

SKYPAT_F(Runtime_f64_reinterpret_i64, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *value1 = new_f64Value(0.375);
    push_Value(stack, value1);

    // Run
    runtime_i64_reinterpret_f64(stack);

    // Check
    Value *check = NULL;
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.i64, 0x3FD8000000000000);

    // Clean
    free_Value(check);
    free_Stack(stack);
}
