#ifndef WASMVM_INSTANCE_DATAINST
#define WASMVM_INSTANCE_DATAINST

#include <defines.h>
#include <dataTypes/vector_t.h>

typedef struct {
    vector_t(byte_t) data;
} DataInst;

#endif