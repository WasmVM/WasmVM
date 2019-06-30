#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <stdlib.h>
#include <math.h>
}
#undef _Bool

SKYPAT_F(Runtime_f32_demote_f64, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f64Value(-22.5));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -22.5);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(22.5));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 22.5);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(nan("")));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(strtod("INF", NULL)));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(0));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0));
    runtime_f32_demote_f64(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -0);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
