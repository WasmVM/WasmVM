#ifndef WASMVM_DATATYPES_FUNCTYPE_DEF
#define WASMVM_DATATYPES_FUNCTYPE_DEF

#include <dataTypes/vector_p.h>

typedef struct FuncType_ {
    vector_p params;     // ValueType
    vector_p results;    // ValueType
} * FuncType;

FuncType new_FuncType();
void free_FuncType(FuncType funcType);
void clean_FuncType(FuncType funcType);

#endif