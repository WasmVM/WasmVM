#ifndef WASMVM_STRUCTURE_ELEM
#define WASMVM_STRUCTURE_ELEM

#include <dataTypes/Value.h>

typedef struct _wasm_elem {
    uint32_t    table;
    Value       offset;
    uint32_t*   init;
} WasmElem;

#endif