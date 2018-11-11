#include <instance/VariableInstrInst.h>

#include <stdlib.h>

VariableInstrInst* new_VariableInstrInst()
{
    VariableInstrInst* instrInst = (VariableInstrInst*) malloc(sizeof(VariableInstrInst));
    instrInst->parent.opcode = 0;
    instrInst->parent.free = (void(*)(InstrInst*)) free_VariableInstrInst;
    instrInst->index = 0;
    return instrInst;
}
void free_VariableInstrInst(VariableInstrInst* instrInst)
{
    free(instrInst);
}