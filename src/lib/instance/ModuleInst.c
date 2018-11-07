#include <instance/ModuleInst.h>

#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/FuncType.h>
#include <instance/ExportInst.h>

ModuleInst* new_ModuleInst(){
    ModuleInst* newModuleInst = (ModuleInst*) malloc(sizeof(ModuleInst));
    newModuleInst->types = new_vector(sizeof(FuncType), (void(*)(void*))free_FuncType);
    newModuleInst->funcaddrs = new_vector(sizeof(uint32_t), free);
    newModuleInst->tableaddrs = new_vector(sizeof(uint32_t), free);
    newModuleInst->memaddrs = new_vector(sizeof(uint32_t), free);
    newModuleInst->globaladdrs = new_vector(sizeof(uint32_t), free);
    newModuleInst->exports = new_vector(sizeof(ExportInst), (void(*)(void*))free_ExportInst);
    return newModuleInst;
}

void free_ModuleInst(ModuleInst* moduleInst){
    free_vector(moduleInst->types);
    free_vector(moduleInst->funcaddrs);
    free_vector(moduleInst->tableaddrs);
    free_vector(moduleInst->memaddrs);
    free_vector(moduleInst->globaladdrs);
    free_vector(moduleInst->exports);
    free(moduleInst);
}
