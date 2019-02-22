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
        uint32_t typeidx; // index of funcType in types
        struct {
            uint32_t min;
            uint32_t max;
        } limits; // table, memory
        struct {
            _Bool mut;
            ValueType valueType;
        } global;
    } desc;
} WasmImport;

WasmImport* new_WasmImport(char* moduleName, char* name);
void clean_WasmImport(WasmImport* import);
void free_WasmImport(WasmImport* import);

#endif