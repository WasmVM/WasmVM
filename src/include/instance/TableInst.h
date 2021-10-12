#ifndef WASMVM_INSTANCE_TABLEINST
#define WASMVM_INSTANCE_TABLEINST

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/RefType.h>

typedef struct {
    u32_t min;
    u32_t max;
    RefType type;
    vector_t(Ref) elem; // uint32_t
} TableInst;

#endif