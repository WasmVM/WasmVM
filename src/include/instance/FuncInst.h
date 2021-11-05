#ifndef WASMVM_INSTANCE_FUNCINST
#define WASMVM_INSTANCE_FUNCINST

#include <Stack.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>
#include <instance/InstrInst.h>

typedef int (*hostfunc_t) (Stack* stack, void* data);

typedef struct {
    FuncType type;
    vector_t(ValueType) locals;
    enum {
        FuncBody_Wasm,
        FuncBody_Host,
    } bodyType;
    union {
        struct {
            ModuleInst* module;
            vector_t(InstrInst*) code;
        } wasm;
        hostfunc_t hostcode;
    } body;
} FuncInst;

#endif