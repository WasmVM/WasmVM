#ifndef WASMVM_INSTANCE_EXPORTINST
#define WASMVM_INSTANCE_EXPORTINST

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/DescType.h>

typedef struct {
    vector_t(byte_t) name;
    DescType type;
    u32_t value;
} ExportInst;

#endif