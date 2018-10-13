#include <instance/TableInst.h>

TableInst* new_TableInst(){
    TableInst* newTableInst = (TableInst*)malloc(sizeof(TableInst));
    newTableInst->elem = new_vector(sizeof(uint32_t), free);
    newTableInst->max = 0;
    return newTableInst;
}

void free_TableInst(TableInst* tableInst){
    free_vector(tableInst->elem);
    free(tableInst);
}