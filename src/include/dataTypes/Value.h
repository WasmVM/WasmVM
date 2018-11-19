#ifndef WASMVM_DATATYPES_VALUE_DEF
#define WASMVM_DATATYPES_VALUE_DEF

#include <stdint.h>
#include <dataTypes/Entry.h>

typedef enum {
    Value_Unspecified = 0,
    Value_i32,
    Value_i64,
    Value_f32,
    Value_f64
} ValueType;

typedef struct {
    Entry parent;
    ValueType type;
    union {
        int32_t i32;
        int64_t i64;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
    } value;
} Value;

Value* new_i32Value(int32_t value);
Value* new_i64Value(int64_t value);
Value* new_f32Value(float value);
Value* new_f64Value(double value);
void free_Value(Value *value);

#endif