#ifndef WASMVM_INSTANCE_FUNCINST
#define WASMVM_INSTANCE_FUNCINST

#include <Stack.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>
#include <instance/InstrInst.h>

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
        int (*hostcode)(Stack* stack, void* data);
    } body;
} FuncInst;

#endif