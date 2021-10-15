#include <defines.h>
#include <dataTypes/vector_t.h>
#include <instance/ModuleInst.h>

void module_inst_free(ModuleInst* moduleInst)
{
    if(moduleInst != NULL) {
        // types
        for(unsigned int i = 0; i < moduleInst->types.size; ++i) {
            free_FuncType(moduleInst->types.data + i);
        }
        free_vector(moduleInst->types);
        // Export
        for(unsigned int i = 0; i < moduleInst->exports.size; ++i) {
            free_ExportInst(moduleInst->exports.data + i);
        }
        free_vector(moduleInst->exports);
        // Addrs
        free_vector(moduleInst->funcaddrs);
        free_vector(moduleInst->tableaddrs);
        free_vector(moduleInst->memaddrs);
        free_vector(moduleInst->globaladdrs);
        // self
        free_func(moduleInst);
    }
}

void free_ExportInst(ExportInst* export)
{
    if(export != NULL) {
        free_vector(export->name);
        free_func(export);
    }
}