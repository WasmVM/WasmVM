#ifndef WASMVM_STRUCTURE_GLOBAL
#define WASMVM_STRUCTURE_GLOBAL

#include <dataTypes/Value.h>

typedef struct _wasm_global {
    _Bool       mut;
    ValueType   valType;
    Value       init;
} WasmGlobal;

#endif