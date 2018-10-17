#ifndef WASMVM_INSTANCE_EXPORTINST_DEF
#define WASMVM_INSTANCE_EXPORTINST_DEF

#include <stdint.h>
#include <dataTypes/DescType.h>

typedef struct {
    char* name;
    DescType descType;
    uint32_t valueAddr;
} ExportInst;
void free_ExportInst(ExportInst* exportInst);

#endif