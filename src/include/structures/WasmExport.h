#ifndef WASMVM_STRUCTURE_EXPORT
#define WASMVM_STRUCTURE_EXPORT

#include <defines.h>
#include <dataTypes/DescType.h>

typedef struct {
    vector_t(byte_t) name;
    DescType         descType;
    u32_t            descIdx;
} WasmExport;

#endif