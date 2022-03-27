#ifndef WASMVM_DATATYPES_VALUE_DEF
#define WASMVM_DATATYPES_VALUE_DEF

#include <defines.h>

typedef enum {
    Value_unspecified = 0,
    Value_i32,
    Value_i64,
    Value_f32,
    Value_f64,
    Value_funcref,
    Value_externref,
    Value_index,
} ValueType;

typedef struct {
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

typedef Value wasm_value;

#endif