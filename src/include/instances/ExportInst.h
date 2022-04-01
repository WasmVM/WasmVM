#ifndef WASMVM_INSTANCE_EXPORTINST
#define WASMVM_INSTANCE_EXPORTINST

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/DescType.h>
#include <instances/ExternVal.h>

typedef struct {
    vector_t(byte_t) name;
    ExternVal value;
} ExportInst;

typedef ExportInst wasm_export_inst;

#endif