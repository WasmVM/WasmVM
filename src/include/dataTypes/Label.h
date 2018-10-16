#ifndef WASMVM_DATATYPE_LABEL_DEF
#define WASMVM_DATATYPE_LABEL_DEF

#include <stdint.h>
#include <dataTypes/Entry.h>

typedef struct {
    Entry parent;
    uint32_t funcAddr;
    uint32_t instrIndex;
    uint32_t contInstr;
} Label;

#endif