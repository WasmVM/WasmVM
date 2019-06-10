#include <instance/ControlInstrInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

ControlInstrInst* new_ControlInstrInst()
{
    ControlInstrInst* instrInst = (ControlInstrInst*) malloc(sizeof(ControlInstrInst));
    instrInst->parent.free = (void(*)(InstrInst*))free_ControlInstrInst;
    instrInst->parent.clean = (void(*)(InstrInst*))clean_ControlInstrInst;
    instrInst->resultTypes = new_vector_p(ValueType, NULL);
    instrInst->indices = new_vector_p(uint32_t, NULL);
    return instrInst;
}
void clean_ControlInstrInst(ControlInstrInst* instance)
{
    free_vector_p(instance->resultTypes);
    free_vector_p(instance->indices);
}

void free_ControlInstrInst(ControlInstrInst* instance)
{
    clean_ControlInstrInst(instance);
    free(instance);
}
