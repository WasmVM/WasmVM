#include <instance/ExportInst.h>

#include <stdlib.h>

ExportInst* new_ExportInst()
{
    ExportInst* instance = (ExportInst*)malloc(sizeof(ExportInst));
    instance->descType = Desc_Unspecified;
    instance->name = NULL;
    instance->valueAddr = 0;
    return instance;
}

void clean_ExportInst(ExportInst* exportInst)
{
    free(exportInst->name);
}

void free_ExportInst(ExportInst* exportInst)
{
    clean_ExportInst(exportInst);
    free(exportInst);
}