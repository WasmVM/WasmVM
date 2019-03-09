#ifndef WASMVM_DATATYPE_LABEL_DEF
#define WASMVM_DATATYPE_LABEL_DEF

#include <stdint.h>
#include <dataTypes/Entry.h>

typedef struct {
    Entry parent;
    uint32_t funcAddr;
    uint32_t instrIndex; // what's the number of this Label
    uint32_t contInstr;  // where will it go, if we meet a if
} Label;

Label* new_Label(uint32_t funcAddr, uint32_t instrIndex, uint32_t contInstr);
void free_Label(Label *label);

#endif