#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <stdlib.h>
#include <math.h>
}
#undef _Bool

SKYPAT_F(Runtime_f64_promote_f32, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f32Value(-22.5));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -22.5);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(22.5));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 22.5);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(nanf("")));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(strtof("INF", NULL)));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(0));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-0));
    runtime_f64_promote_f32(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -0);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
