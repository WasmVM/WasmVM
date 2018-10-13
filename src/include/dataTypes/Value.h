#ifndef WASMVM_DATATYPES_VALUE_DEF
#define WASMVM_DATATYPES_VALUE_DEF

#include <stdint.h>
#include <dataTypes/Entry.h>

typedef enum{
    Value_Unspecified = 0,
    Value_i32,
    Value_i64,
    Value_f32,
    Value_f64
} ValueType;

typedef struct {
    Entry parent;
    ValueType type;
    union{
        int32_t i32;
        int64_t i64;
        float f32;
        double f64;
    }value;
} Value;

#endif