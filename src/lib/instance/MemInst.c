#include <instance/MemInst.h>

#include <stdlib.h>

MemInst* new_MemInst(){
    MemInst* newMemInst = (MemInst*) malloc(sizeof(MemInst));
    newMemInst->data = new_vector(sizeof(char), NULL);
    newMemInst->max = 0;
    return newMemInst;
}
void free_MemInst(MemInst* memInst){
    free_vector(memInst->data);
    free(memInst);
}