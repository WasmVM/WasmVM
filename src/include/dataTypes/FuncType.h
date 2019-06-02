#ifndef WASMVM_DATATYPES_FUNCTYPE_DEF
#define WASMVM_DATATYPES_FUNCTYPE_DEF

#include <dataTypes/vector.h>

typedef struct FuncType_ {
    vector* params;     // ValueType
    vector* results;    // ValueType
} * FuncType;

FuncType new_FuncType();
void free_FuncType(FuncType funcType);
void clean_FuncType(FuncType funcType);

#endif