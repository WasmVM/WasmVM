#ifndef WASMVM_INSTANCE_FUNCINST_DEF
#define WASMVM_INSTANCE_FUNCINST_DEF

#include <dataTypes/vector_p.h>
#include <dataTypes/list_p.h>
#include <dataTypes/FuncType.h>
#include <instance/ModuleInst.h>

typedef struct {
    FuncType type;
    vector_p locals;    // ValueType
    ModuleInst* module;
    list_p code;    // InstrInst
    // intro of hostcode https://webassembly.github.io/spec/core/exec/runtime.html#function-instances
    int (*hostcode)(); // TODO: accelerator
} FuncInst;

FuncInst* new_FuncInst(ModuleInst* module, FuncType type);
void clean_FuncInst(FuncInst* funcInst);
void free_FuncInst(FuncInst* funcInst);

#endif