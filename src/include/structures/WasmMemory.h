#ifndef WASMVM_STRUCTURE_MEMORY
#define WASMVM_STRUCTURE_MEMORY

#include <dataTypes/Value.h>

typedef struct _wasm_func {
    uint32_t    min;
    uint32_t    max;
} WasmMemory;

#endif