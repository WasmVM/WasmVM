#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Runtime.h>
#include <stdlib.h>
#include <math.h>
}
#undef _Bool

SKYPAT_F(runtime_f32_mul, regular)
{
    // Prepare
    Stack stack = new_Stack();
    Value *check = NULL;

    push_Value(stack, new_f32Value(+nanf("")));
    push_Value(stack, new_f32Value(3));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(5));
    push_Value(stack, new_f32Value(+nanf("")));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    push_Value(stack, new_f32Value(+0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    push_Value(stack, new_f32Value(-0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    push_Value(stack, new_f32Value(-0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    push_Value(stack, new_f32Value(+0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);
    check = NULL;
    push_Value(stack, new_f32Value(+0.0f));
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-0.0f));
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+0.0f));
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-0.0f));
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_TRUE(isnan(check->value.f32));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(3.14));
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(3.14));
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+strtof("INF", NULL)));
    push_Value(stack, new_f32Value(3.14));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-strtof("INF", NULL)));
    push_Value(stack, new_f32Value(3.14));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -strtof("INF", NULL));
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+0.0f));
    push_Value(stack, new_f32Value(-0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -0.0f);
    EXPECT_EQ(signbit(check->value.f32), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-0.0f));
    push_Value(stack, new_f32Value(+0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -0.0f);
    EXPECT_EQ(signbit(check->value.f32), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(+0.0f));
    push_Value(stack, new_f32Value(+0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +0.0f);
    EXPECT_EQ(signbit(check->value.f32), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(-0.0f));
    push_Value(stack, new_f32Value(-0.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, +0.0f);
    EXPECT_EQ(signbit(check->value.f32), 0);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(3.13f));
    push_Value(stack, new_f32Value(-3.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, -9.39f);
    EXPECT_EQ(signbit(check->value.f32), 1);
    free_Value(check);

    check = NULL;
    push_Value(stack, new_f32Value(3.13f));
    push_Value(stack, new_f32Value(3.0f));
    runtime_f32_mul(stack);
    pop_Value(stack,&check);
    EXPECT_EQ(check->value.f32, 9.39f);
    free_Value(check);

    // Clean
    free_Stack(stack);
}
