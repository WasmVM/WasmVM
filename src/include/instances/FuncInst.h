#ifndef WASMVM_INSTANCE_FUNCINST
#define WASMVM_INSTANCE_FUNCINST

#include <Stack.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <instances/ModuleInst.h>
#include <instances/InstrInst.h>

typedef int (*hostfunc_t) (Stack* stack, void* data);

typedef struct {
    FuncType* type;
    enum {
        FuncBody_Wasm,
        FuncBody_Host,
    } bodyType;
    union {
        struct {
            vector_t(ValueType) locals;
            ModuleInst* module;
            vector_t(byte_t) codes;
        } wasm;
        hostfunc_t hostcode;
    } body;
} FuncInst;

#endif