#ifndef WASMVM_STRUCTURE_ELEM
#define WASMVM_STRUCTURE_ELEM

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>

typedef struct _wasm_elem {
    uint32_t    table;
    Value       offset;
    vector*     init;       // uint32_t
} WasmElem;

#endif