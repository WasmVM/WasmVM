#ifndef WASMVM_STRUCTURE_TABLE
#define WASMVM_STRUCTURE_TABLE

#include <defines.h>
#include <dataTypes/RefType.h>

typedef struct {
    u32_t    min;
    u32_t    max;
    RefType  refType;
} WasmTable;

typedef WasmTable wasm_table;

#endif