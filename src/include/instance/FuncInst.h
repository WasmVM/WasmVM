#ifndef WASMVM_INSTANCE_FUNCINST
#define WASMVM_INSTANCE_FUNCINST

// #include <dataTypes/vector_p.h>
// #include <dataTypes/list_p.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>
#include <instance/InstrInst.h>

typedef struct {
    FuncType type;
    vector_t(ValueType) locals;    // ValueType
    ModuleInst* module;
    // int (*hostcode)(); // TODO: accelerator
    vector_t(InstrInst*) code;    // InstrInst
    byte_t payload[];
} FuncInst;

#endif