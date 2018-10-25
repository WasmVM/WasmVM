#include <instance/InstrInst.h>

#include <stdlib.h>

InstrInst* new_InstrInst(){
    InstrInst* instrInst = (InstrInst*) malloc(sizeof(InstrInst));
    instrInst->opcode = 0;
    instrInst->free = (void(*)(InstrInst*))free;
    return instrInst;
}

void free_InstrInst(InstrInst* instrInst){
    instrInst->free(instrInst);
}