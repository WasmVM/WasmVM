#ifndef WASMVM_STRUCTURE_GLOBAL
#define WASMVM_STRUCTURE_GLOBAL

#include <dataTypes/ConstExpr.h>

typedef struct {
    _Bool       mut;
    ValueType   valType;
    ConstExpr   init;
} WasmGlobal;

typedef WasmGlobal wasm_global;

#endif