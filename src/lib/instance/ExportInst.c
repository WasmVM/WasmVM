#include <instance/ExportInst.h>

void free_ExportInst(ExportInst* exportInst){
    free(exportInst->name);
    free(exportInst);
}