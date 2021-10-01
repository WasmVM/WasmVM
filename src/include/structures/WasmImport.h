#ifndef WASMVM_STRUCTURE_IMPORT
#define WASMVM_STRUCTURE_IMPORT

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>
#include <dataTypes/vector_t.h>

typedef struct {
    vector_t(byte_t) module;
    vector_t(byte_t) name;
    DescType         descType;
    union {
        u32_t typeidx; // index of funcType in types
        struct {
            u32_t min;
            u32_t max;
        } limits; // table, memory
        struct {
            _Bool mut;
            ValueType valueType;
        } global;
    } desc;
} WasmImport;

// WasmImport* new_WasmImport(char* moduleName, char* name);
// void clean_WasmImport(WasmImport* import);
// void free_WasmImport(WasmImport* import);

#endif