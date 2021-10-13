#ifndef WASMVM_INSTANCE_MEMINST
#define WASMVM_INSTANCE_MEMINST

#include <defines.h>
#include <dataTypes/vector_t.h>

typedef struct {
    u32_t max;
    vector_t(byte_t) data;
} MemInst;

#endif