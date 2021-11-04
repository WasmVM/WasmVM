#ifndef WASMVM_INSTANCE_MODULEINST_DEF
#define WASMVM_INSTANCE_MODULEINST_DEF

#include <defines.h>
#include <dataTypes/vector_t.h>
#include <dataTypes/FuncType.h>
#include <instance/ExportInst.h>

typedef struct {
    vector_t(FuncType) types;
    vector_t(u32_t) funcaddrs;
    vector_t(u32_t) tableaddrs;
    vector_t(u32_t) memaddrs;
    vector_t(u32_t) globaladdrs;
    vector_t(u32_t) elemaddrs;
    vector_t(u32_t) dataaddrs;
    vector_t(ExportInst) exports;
} ModuleInst;

typedef ModuleInst* wasm_module_inst;

void module_inst_free(wasm_module_inst moduleInst);

#endif