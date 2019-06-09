#include "Label_.h"

#include <stdlib.h>

Label new_Label(uint32_t funcAddr, uint32_t instrIndex, uint32_t contInstr)
{
    Label label = (Label) malloc(sizeof(struct Label_));
    label->parent.entryType = Entry_Label;
    label->funcAddr = funcAddr;
    label->instrIndex = instrIndex;
    label->contInstr = contInstr;
    label->endInstr = contInstr;
    label->resultTypes = NULL;
    return label;
}
void free_Label(Label label)
{
    free(label);
}

uint32_t label_get_funcAddr(Label label)
{
    return label->funcAddr;
}
uint32_t label_get_instrIndex(Label label)
{
    return label->instrIndex;
}
void label_set_instrIndex(Label label, uint32_t instrIndex)
{
    label->instrIndex = instrIndex;
}
uint32_t label_get_contInstr(Label label)
{
    return label->contInstr;
}
uint32_t label_get_endInstr(Label label)
{
    return label->endInstr;
}
void label_set_endInstr(Label label, uint32_t endInstr)
{
    label->endInstr = endInstr;
}
vector_p label_get_resultTypes(Label label)
{
    return label->resultTypes;
}
void label_set_resultTypes(Label label, vector_p resultTypes)
{
    label->resultTypes = resultTypes;
}