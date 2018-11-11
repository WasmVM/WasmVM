#include <instance/NumericInstrInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

NumericInstrInst* new_NumericInstrInst()
{
    NumericInstrInst* instrInst = (NumericInstrInst*) malloc(sizeof(NumericInstrInst));
    instrInst->constant.parent.entryType = Entry_Value;
    instrInst->constant.type = Value_Unspecified;
    instrInst->parent.opcode = 0;
    instrInst->parent.free = (void(*)(InstrInst*)) free_NumericInstrInst;
    return instrInst;
}
void free_NumericInstrInst(NumericInstrInst* instrInst)
{
    free(instrInst);
}