#ifndef WASMVM_INSTANCE_FUNCINST_DEF
#define WASMVM_INSTANCE_FUNCINST_DEF

#include <dataTypes/vector.h>
#include <dataTypes/FuncType.h>
#include <instance/ModuleInst.h>

typedef struct {
    FuncType* type;
    vector* locals;    // ValueType
    ModuleInst* module;
    vector* code;    // InstrInst
    int (*hostcode)(); // TODO: accelerator
} FuncInst;

FuncInst* new_FuncInst(ModuleInst* module, FuncType* type);
void clean_FuncInst(FuncInst* funcInst);
void free_FuncInst(FuncInst* funcInst);

#endif