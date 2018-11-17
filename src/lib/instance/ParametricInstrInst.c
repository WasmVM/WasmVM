#include <instance/ParametricInstrInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

ParametricInstrInst* new_ParametricInstrInst()
{
    ParametricInstrInst* instrInst = (ParametricInstrInst*) malloc(sizeof(ParametricInstrInst));
    instrInst->opcode = 0;
    instrInst->free = (void(*)(InstrInst*)) free_ParametricInstrInst;
    return instrInst;
}
void free_ParametricInstrInst(ParametricInstrInst* instrInst)
{
    free(instrInst);
}