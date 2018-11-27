#ifndef WASMVM_STRUCTURE_IMPORT
#define WASMVM_STRUCTURE_IMPORT

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_import {
    char*       module;
    char*       name;
    DescType    descType;
    union {
        uint32_t typeidx;
        struct {
            uint32_t min;
            uint32_t max;
        } limits;
        Value value;
    } desc;

} WasmImport;

#endif