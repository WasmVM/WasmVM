#ifndef WASMVM_INSTANCE_EXTERNVAL
#define WASMVM_INSTANCE_EXTERNVAL

#include <defines.h>
#include <dataTypes/DescType.h>

typedef struct {
    DescType type;
    u32_t value;
} ExternVal;

#endif