#ifndef WASMVM_STRUCTURE_IMPORT
#define WASMVM_STRUCTURE_IMPORT

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_import {
    vector*     module;     // char
    vector*     name;       // char
    DescType    descType;    
    uint32_t    descIdex;
} WasmImport;

#endif