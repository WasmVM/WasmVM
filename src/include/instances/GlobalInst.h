#ifndef WASMVM_INSTANCE_GLOBAL
#define WASMVM_INSTANCE_GLOBAL

#include <dataTypes/Value.h>

typedef struct {
    _Bool mut;
    Value val;
} GlobalInst;

typedef GlobalInst wasm_global_type;

#endif