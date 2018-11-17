#include <instance/MemoryInstrInst.h>

#include <stdlib.h>

MemoryInstrInst* new_MemoryInstrInst()
{
    MemoryInstrInst* instrInst = (MemoryInstrInst*) malloc(sizeof(MemoryInstrInst));
    instrInst->offset = 0;
    instrInst->align = 0;
    instrInst->parent.opcode = 0;
    instrInst->parent.free = (void(*)(InstrInst*)) free_MemoryInstrInst;
    return instrInst;
}
void free_MemoryInstrInst(MemoryInstrInst* instrInst)
{
    free(instrInst);
}