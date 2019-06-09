#ifndef WASMVM_DATATYPES_LABEL__DEF
#define WASMVM_DATATYPES_LABEL__DEF

#include <dataTypes/Label.h>
#include <stdint.h>
#include <dataTypes/Entry.h>
#include <dataTypes/vector_p.h>

struct Label_ {
    Entry parent;
    uint32_t funcAddr;
    uint32_t instrIndex;
    uint32_t contInstr;
    uint32_t endInstr;
    vector_p resultTypes; // ValueType
};

#endif