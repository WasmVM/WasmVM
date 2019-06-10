#include <instance/TableInst.h>

#include <stdint.h>
#include <stdlib.h>

TableInst* new_TableInst()
{
    TableInst* newTableInst = (TableInst*)malloc(sizeof(TableInst));
    newTableInst->elem = new_vector_p(uint32_t, NULL);
    newTableInst->max = 0;
    return newTableInst;
}

void clean_TableInst(TableInst* tableInst)
{
    free_vector_p(tableInst->elem);
}

void free_TableInst(TableInst* tableInst)
{
    clean_TableInst(tableInst);
    free(tableInst);
}
