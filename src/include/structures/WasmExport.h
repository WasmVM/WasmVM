#ifndef WASMVM_STRUCTURE_EXPORT
#define WASMVM_STRUCTURE_EXPORT

#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_export {
    char*       name;
    DescType    descType;
    uint32_t    descIdx;
} WasmExport;

#endif