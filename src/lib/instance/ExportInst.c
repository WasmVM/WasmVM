#include <instance/ExportInst.h>

#include <stdlib.h>

void free_ExportInst(ExportInst* exportInst){
    free(exportInst->name);
    free(exportInst);
}