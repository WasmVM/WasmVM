#ifndef WASMVM_INSTANCE_MODULEINST_DEF
#define WASMVM_INSTANCE_MODULEINST_DEF

#include <dataTypes/vector_p.h>

typedef struct ModuleInst_ {
    char* name;
    vector_p types;       // FuncType
    // funcaddrs is static index of vector_p funcs in Store
    vector_p funcaddrs;   // uint32_t
    vector_p tableaddrs;  // uint32_t
    vector_p memaddrs;    // uint32_t
    vector_p globaladdrs; // uint32_t
    vector_p exports;     // ExportInst
} ModuleInst;

ModuleInst* new_ModuleInst(char* name);
void clean_ModuleInst(ModuleInst* moduleInst);
void free_ModuleInst(ModuleInst* moduleInst);

#endif