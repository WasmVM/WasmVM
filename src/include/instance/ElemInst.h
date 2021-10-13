#ifndef WASMVM_INSTANCE_ELEMINST
#define WASMVM_INSTANCE_ELEMINST

#include <dataTypes/vector_t.h>
#include <dataTypes/RefType.h>

typedef struct {
    RefType type;
    vector_t(Ref) elem;
} ElemInst;

#endif