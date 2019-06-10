#ifndef WASMVM_STRUCTURE_DATA
#define WASMVM_STRUCTURE_DATA

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>

typedef struct _wasm_data {
    uint32_t    data;
    Value       offset;
    vector_p     init;       // char
} WasmData;

WasmData* new_WasmData();
void clean_WasmData(WasmData* thisWasmData);
void free_WasmData(WasmData* thisWasmData);

#endif