#ifndef WASMVM_STRUCTURE_EXPORT
#define WASMVM_STRUCTURE_EXPORT

#include <stdint.h>
#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>

typedef struct _wasm_export {
    char*       name;
    DescType    descType;
    uint32_t    descIdx;
} WasmExport;

WasmExport* new_WasmExport(char* name, DescType descType, uint32_t descIdx);
void clean_WasmExport(WasmExport *thisWasmExport);
void free_WasmExport(WasmExport *thisWasmExport);

#endif