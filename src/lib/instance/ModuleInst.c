#include <instance/ModuleInst.h>

#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/FuncType.h>
#include <instance/ExportInst.h>

ModuleInst* new_ModuleInst(char* name)
{
    ModuleInst* newModuleInst = (ModuleInst*) malloc(sizeof(ModuleInst));
    newModuleInst->name = name;
    newModuleInst->types = new_vector(sizeof(FuncType), (void(*)(void*))clean_FuncType);
    newModuleInst->funcaddrs = new_vector(sizeof(uint32_t), NULL);
    newModuleInst->tableaddrs = new_vector(sizeof(uint32_t), NULL);
    newModuleInst->memaddrs = new_vector(sizeof(uint32_t), NULL);
    newModuleInst->globaladdrs = new_vector(sizeof(uint32_t), NULL);
    newModuleInst->exports = new_vector(sizeof(ExportInst), (void(*)(void*))(clean_ExportInst));
    return newModuleInst;
}

void clean_ModuleInst(ModuleInst* moduleInst)
{
    free(moduleInst->name);
    free_vector(moduleInst->types);
    free_vector(moduleInst->funcaddrs);
    free_vector(moduleInst->tableaddrs);
    free_vector(moduleInst->memaddrs);
    free_vector(moduleInst->globaladdrs);
    free_vector(moduleInst->exports);
}

void free_ModuleInst(ModuleInst* moduleInst)
{
    clean_ModuleInst(moduleInst);
    free(moduleInst);
}
