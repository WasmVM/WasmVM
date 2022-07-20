#ifndef WASMVM_STRUCTURE_IMPORT
#define WASMVM_STRUCTURE_IMPORT

#include <defines.h>
#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/RefType.h>

typedef struct {
    vector_t(byte_t) module;
    vector_t(byte_t) name;
    DescType         descType;
    union {
        u32_t typeidx; // index of funcType in types
        struct {
            u32_t min;
            u32_t max;
            RefType type;
        } limits; // table, memory
        struct {
            _Bool mut;
            ValueType valueType;
        } global;
    } desc;
} WasmImport;

#endif