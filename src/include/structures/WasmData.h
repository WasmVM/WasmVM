#ifndef WASMVM_STRUCTURE_DATA
#define WASMVM_STRUCTURE_DATA

#include <dataTypes/Value.h>

typedef struct _wasm_data {
    uint32_t    data;
    Value       offset;
    char*       init;
} WasmData;

#endif