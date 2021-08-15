#ifndef WASMVM_STRUCTURE_GLOBAL
#define WASMVM_STRUCTURE_GLOBAL

#include <dataTypes/Value.h>

typedef struct {
    _Bool       mut;
    ValueType   valType;
    Value       init;
    // TODO: global.get init
} WasmGlobal;

#endif