#include <instance/ControlInstrInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

ControlInstrInst* new_ControlInstrInst(){
    ControlInstrInst* instrInst = (ControlInstrInst*) malloc(sizeof(ControlInstrInst));
    instrInst->parent.free = (void(*)(InstrInst*))free_ControlInstrInst;
    instrInst->resultType = Value_Unspecified;
    instrInst->indices = new_vector(sizeof(uint32_t), free);
    return instrInst;
}
void free_ControlInstrInst(ControlInstrInst* instance){
    free_vector(instance->indices);
    free(instance);
}