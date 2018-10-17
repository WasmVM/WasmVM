#include <instance/GlobalInst.h>

#include <stdlib.h>

GlobalInst* new_GlobalInst(){
    return (GlobalInst*) malloc(sizeof(GlobalInst));
}

void free_GlobalInst(GlobalInst* globalInst){
    free(globalInst);
}