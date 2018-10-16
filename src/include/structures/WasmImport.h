#ifndef WASMVM_STRUCTURE_IMPORT
#define WASMVM_STRUCTURE_IMPORT

#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_import {
    char*       module;
    char*       name; 
    DescType    descType;    
    uint32_t    descIdex;
} WasmImport;

#endif