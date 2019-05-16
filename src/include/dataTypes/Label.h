#ifndef WASMVM_DATATYPE_LABEL_DEF
#define WASMVM_DATATYPE_LABEL_DEF

#include <stdint.h>
#include <dataTypes/Entry.h>
#include <dataTypes/vector.h>

typedef struct {
    Entry parent;
    uint32_t funcAddr;
    uint32_t instrIndex;
    uint32_t contInstr;
    uint32_t endInstr;
    vector* resultTypes; // ValueType
} Label;

Label* new_Label(uint32_t funcAddr, uint32_t instrIndex, uint32_t contInstr);
void free_Label(Label *label);

#endif