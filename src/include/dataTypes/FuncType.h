#ifndef WASMVM_DATATYPES_FUNCTYPE_DEF
#define WASMVM_DATATYPES_FUNCTYPE_DEF

#include <dataTypes/vector_t.h>
#include <dataTypes/Value.h>

typedef struct {
    vector_t(ValueType) params;     // ValueType
    vector_t(ValueType) results;    // ValueType
} FuncType;

void free_FuncType(FuncType* type);

#endif