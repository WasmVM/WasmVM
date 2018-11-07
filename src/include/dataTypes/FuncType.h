#ifndef WASMVM_INSTANCE_FUNCTYPE_DEF
#define WASMVM_INSTANCE_FUNCTYPE_DEF

#include <dataTypes/vector.h>

typedef struct {
    vector* params;     // ValueType
    vector* results;    // ValueType
} FuncType;

FuncType* new_FuncType();
void free_FuncType(FuncType* funcType);

#endif