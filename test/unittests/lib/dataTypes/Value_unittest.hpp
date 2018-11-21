#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <stdlib.h>
}
#undef _Bool

SKYPAT_F(Value, new_i32_value)
{
    // Prepare
    Value* value = new_i32Value(1024);

    // Check
    EXPECT_EQ(value->parent.entryType, Entry_Value);
    EXPECT_EQ(value->type, Value_i32);
    EXPECT_EQ(value->value.i32, 1024);

    free_Value(value);
}

SKYPAT_F(Value, new_i64_value)
{
    // Prepare
    Value* value = new_i64Value(1234);

    // Check
    EXPECT_EQ(value->parent.entryType, Entry_Value);
    EXPECT_EQ(value->type, Value_i64);
    EXPECT_EQ(value->value.i64, 1234);

    free_Value(value);
}

SKYPAT_F(Value, new_f32_value)
{
    // Prepare
    Value* value = new_f32Value(2.5);

    // Check
    EXPECT_EQ(value->parent.entryType, Entry_Value);
    EXPECT_EQ(value->type, Value_f32);
    EXPECT_EQ(value->value.f32, 2.5);

    free_Value(value);
}

SKYPAT_F(Value, new_f64_value)
{
    // Prepare
    Value* value = new_f64Value(1.59);

    // Check
    EXPECT_EQ(value->parent.entryType, Entry_Value);
    EXPECT_EQ(value->type, Value_f64);
    EXPECT_EQ(value->value.f64, 1.59);

    free_Value(value);
}
