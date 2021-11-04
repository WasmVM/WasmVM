#ifndef WASMVM_INSTANCE_EXTERNTYPE
#define WASMVM_INSTANCE_EXTERNTYPE

#include <defines.h>
#include <dataTypes/DescType.h>
#include <dataTypes/FuncType.h>

typedef struct {
    vector_t(byte_t) module;
    vector_t(byte_t) name;
    DescType         descType;
    union {
        FuncType func;
        struct {
            u32_t min;
            u32_t max;
        } limits; // table, memory
        struct {
            _Bool mut;
            ValueType valueType;
        } global;
    } desc;
} ExternType;

typedef ExternType wasm_externtype;

#endif