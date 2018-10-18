#ifndef WASMVM_STRUCTURE_MEMORY
#define WASMVM_STRUCTURE_MEMORY

#include <stdint.h>

typedef struct _wasm_memory {
    uint32_t    min;
    uint32_t    max;
} WasmMemory;

#endif