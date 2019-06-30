#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <stdlib.h>
#include <math.h>
}
#undef _Bool

SKYPAT_F(runtime_f64_div, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f64Value(+nan("")));
    push_Value(stack, new_f64Value(3));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(5));
    push_Value(stack, new_f64Value(+nan("")));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+0.0));
    push_Value(stack, new_f64Value(-0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.0));
    push_Value(stack, new_f64Value(+0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+0.0));
    push_Value(stack, new_f64Value(+0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.0));
    push_Value(stack, new_f64Value(-0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f64));
    free_Value(check);


    check = NULL;
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    push_Value(stack, new_f64Value(+0.5));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    push_Value(stack, new_f64Value(-0.5));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    push_Value(stack, new_f64Value(-0.5));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    push_Value(stack, new_f64Value(+0.5));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -strtod("INF", NULL));
    free_Value(check);
    check = NULL;
    push_Value(stack, new_f64Value(+0.5));
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +0.0);
    EXPECT_EQ(signbit(check->value.f64), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.5));
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +0.0);
    EXPECT_EQ(signbit(check->value.f64), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+0.5));
    push_Value(stack, new_f64Value(-strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -0.0);
    EXPECT_EQ(signbit(check->value.f64), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.5));
    push_Value(stack, new_f64Value(+strtod("INF", NULL)));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -0.0);
    EXPECT_EQ(signbit(check->value.f64), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(3.14));
    push_Value(stack, new_f64Value(+0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-3.14));
    push_Value(stack, new_f64Value(-0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(3.14));
    push_Value(stack, new_f64Value(-0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-3.14));
    push_Value(stack, new_f64Value(+0.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -strtod("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+0.0));
    push_Value(stack, new_f64Value(3.14));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +0.0);
    EXPECT_EQ(signbit(check->value.f64), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.0));
    push_Value(stack, new_f64Value(-3.14));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, +0.0);
    EXPECT_EQ(signbit(check->value.f64), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(+0.0));
    push_Value(stack, new_f64Value(-3.14));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -0.0);
    EXPECT_EQ(signbit(check->value.f64), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(-0.0));
    push_Value(stack, new_f64Value(3.14));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -0.0);
    EXPECT_EQ(signbit(check->value.f64), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(3.0));
    push_Value(stack, new_f64Value(-3.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, -1.0);
    EXPECT_EQ(signbit(check->value.f64), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f64Value(3.0));
    push_Value(stack, new_f64Value(3.0));
    runtime_f64_div(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f64, 1.0);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
