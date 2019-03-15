#include <instance/GlobalInst.h>

#include <stdlib.h>

GlobalInst* new_GlobalInst()
{
    GlobalInst* globalInst = (GlobalInst*) malloc(sizeof(GlobalInst));
    globalInst->mut = 0;
    globalInst->value.type = Value_Unspecified;
    globalInst->value.parent.entryType = Entry_Value;
    return globalInst;
}

void free_GlobalInst(GlobalInst* globalInst)
{
    free(globalInst);
}