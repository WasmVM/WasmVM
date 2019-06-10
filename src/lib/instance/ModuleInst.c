#include <instance/ModuleInst.h>

#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/FuncType.h>
#include <instance/ExportInst.h>

ModuleInst* new_ModuleInst(char* name)
{
    ModuleInst* newModuleInst = (ModuleInst*) malloc(sizeof(ModuleInst));
    newModuleInst->name = name;
    newModuleInst->types = new_vector_p(struct FuncType_, (void(*)(void*))clean_FuncType);
    newModuleInst->funcaddrs = new_vector_p(uint32_t, NULL);
    newModuleInst->tableaddrs = new_vector_p(uint32_t, NULL);
    newModuleInst->memaddrs = new_vector_p(uint32_t, NULL);
    newModuleInst->globaladdrs = new_vector_p(uint32_t, NULL);
    newModuleInst->exports = new_vector_p(ExportInst, (void(*)(void*))(clean_ExportInst));
    return newModuleInst;
}

void clean_ModuleInst(ModuleInst* moduleInst)
{
    free(moduleInst->name);
    free_vector_p(moduleInst->types);
    free_vector_p(moduleInst->funcaddrs);
    free_vector_p(moduleInst->tableaddrs);
    free_vector_p(moduleInst->memaddrs);
    free_vector_p(moduleInst->globaladdrs);
    free_vector_p(moduleInst->exports);
}

void free_ModuleInst(ModuleInst* moduleInst)
{
    clean_ModuleInst(moduleInst);
    free(moduleInst);
}
