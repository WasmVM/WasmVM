#ifndef WASMVM_STRUCTURE_MEMORY
#define WASMVM_STRUCTURE_MEMORY

#include <defines.h>

typedef struct {
    u32_t    min;
    u32_t    max;
} WasmMemory;

typedef WasmMemory wasm_memory;

#endif