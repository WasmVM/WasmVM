#ifndef WASMVM_STRUCTURE_EXPORT
#define WASMVM_STRUCTURE_EXPORT

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_export {
    vector*     name;       // char
    DescType    descType;
    uint32_t    descIdx;
} WasmExport;

#endif