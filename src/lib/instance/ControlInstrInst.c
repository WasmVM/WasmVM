#include <instance/ControlInstrInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

ControlInstrInst* new_ControlInstrInst()
{
    ControlInstrInst* instrInst = (ControlInstrInst*) malloc(sizeof(ControlInstrInst));
    instrInst->parent.free = (void(*)(InstrInst*))free_ControlInstrInst;
    instrInst->parent.clean = (void(*)(InstrInst*))clean_ControlInstrInst;
    instrInst->resultTypes = new_vector(sizeof(ValueType), NULL);
    instrInst->indices = new_vector(sizeof(uint32_t), NULL);
    return instrInst;
}
void clean_ControlInstrInst(ControlInstrInst* instance)
{
    free_vector(instance->resultTypes);
    free_vector(instance->indices);
}

void free_ControlInstrInst(ControlInstrInst* instance)
{
    clean_ControlInstrInst(instance);
    free(instance);
}
