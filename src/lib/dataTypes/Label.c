#include "dataTypes/Label.h"
#include <stdint.h>
#include <stdlib.h>

Label* new_Label(uint32_t funcAddr, uint32_t instrIndex, uint32_t contInstr)
{
    Label* label = (Label*) malloc(sizeof(Label));
    label->parent.entryType = Entry_Label;
    label->funcAddr = funcAddr;
    label->instrIndex = instrIndex;
    label->contInstr = contInstr;
    label->endInstr = contInstr;
    label->resultTypes = NULL;
    return label;
}
void free_Label(Label *label)
{
    free(label);
}