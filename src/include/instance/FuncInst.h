#ifndef WASMVM_INSTANCE_FUNCINST_DEF
#define WASMVM_INSTANCE_FUNCINST_DEF

#include <dataTypes/vector.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>
#include <instance/InstrInst.h>

typedef struct {
    FuncType type;
    vector* locals;    // ValueType
    ModuleInst* module;
    vector* code;    // InstrInst
    int (*hostcode)();
} FuncInst;
void free_FuncInst(FuncInst* funcInst);

#endif