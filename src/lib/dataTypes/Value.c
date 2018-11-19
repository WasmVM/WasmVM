#include <dataTypes/Value.h>

#include <stdint.h>
#include <stdlib.h>

Value* new_i32Value(int32_t value)
{
    Value* val = (Value*) malloc(sizeof(Value));
    val->parent.entryType = Entry_Value;
    val->type = Value_i32;
    val->value.i32 = value;
    return val;
}
Value* new_i64Value(int64_t value)
{
    Value* val = (Value*) malloc(sizeof(Value));
    val->parent.entryType = Entry_Value;
    val->type = Value_i64;
    val->value.i64 = value;
    return val;
}
Value* new_f32Value(float value)
{
    Value* val = (Value*) malloc(sizeof(Value));
    val->parent.entryType = Entry_Value;
    val->type = Value_f32;
    val->value.f32 = value;
    return val;
}
Value* new_f64Value(double value)
{
    Value* val = (Value*) malloc(sizeof(Value));
    val->parent.entryType = Entry_Value;
    val->type = Value_f64;
    val->value.f64 = value;
    return val;
}
void free_Value(Value *value)
{
    free(value);
}