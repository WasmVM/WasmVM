#ifndef WASMVM_STRUCTURE_ELEM
#define WASMVM_STRUCTURE_ELEM

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>

typedef struct _wasm_elem {
    uint32_t    table;
    Value       offset;
    vector_p     init;       // uint32_t
} WasmElem;

WasmElem* new_WasmElem();
void clean_WasmElem(WasmElem *thisWasmElem);
void free_WasmElem(WasmElem *thisWasmElem);

#endif