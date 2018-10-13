#include <instance/FuncInst.h>

void free_FuncInst(FuncInst* funcInst){
    free_vector(funcInst->locals);
    free_vector(funcInst->code);
}