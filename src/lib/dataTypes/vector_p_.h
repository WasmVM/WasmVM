#ifndef WASMVM_DATATYPES_VECTOR__DEF
#define WASMVM_DATATYPES_VECTOR__DEF

#include <stddef.h>
#include <dataTypes/vector_p.h>

struct vector_p_ {
    void *data;
    size_t length;
    size_t capacity;
    size_t unitSize;
    void (*cleanFunc)(void*);
};

#endif