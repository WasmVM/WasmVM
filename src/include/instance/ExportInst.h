#ifndef WASMVM_INSTANCE_EXPORTINST
#define WASMVM_INSTANCE_EXPORTINST

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/DescType.h>
#include <instance/ExternVal.h>

typedef struct {
    vector_t(byte_t) name;
    ExternVal value;
} ExportInst;

void free_ExportInst(ExportInst* export);

#endif