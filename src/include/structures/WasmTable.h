#ifndef WASMVM_STRUCTURE_TABLE
#define WASMVM_STRUCTURE_TABLE

#include <dataTypes/Value.h>

typedef struct _wasm_table {
    uint32_t    min;
    uint32_t    max;
    uint32_t    elemType;
} WasmTable;

#endif