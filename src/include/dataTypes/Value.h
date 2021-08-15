#ifndef WASMVM_DATATYPES_VALUE_DEF
#define WASMVM_DATATYPES_VALUE_DEF

#include <defines.h>
// #include <dataTypes/Entry.h>

typedef enum {
    Value_Unspecified = 0,
    Value_i32,
    Value_i64,
    Value_f32,
    Value_f64,
    Value_funcref,
    Value_externref,
} ValueType;

typedef struct {
    // Entry parent;
    ValueType type;
    union {
        i32_t i32;
        i64_t i64;
        u32_t u32;
        u64_t u64;
        f32_t f32;
        f64_t f64;
        void* ref;
    } value;
} Value;

// Value* new_i32Value(int32_t value);
// Value* new_i64Value(int64_t value);
// Value* new_f32Value(float value);
// Value* new_f64Value(double value);
// void free_Value(Value *value);

#endif