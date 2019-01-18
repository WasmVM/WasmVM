#include <instance/FuncInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

FuncInst* new_FuncInst()
{
    FuncInst* instance = (FuncInst*) malloc(sizeof(FuncInst));
    instance->type.params = new_vector(sizeof(ValueType));
    instance->type.results = new_vector(sizeof(ValueType));
    instance->locals = new_vector(sizeof(ValueType));
    instance->module = NULL;
    instance->code = new_vector(sizeof(InstrInst));
    instance->hostcode = NULL;
    return instance;
}

void free_FuncInst(FuncInst* funcInst)
{
    free_vector(funcInst->type.params);
    free_vector(funcInst->type.results);
    free_vector(funcInst->locals);
    free_vector(funcInst->code);
    free(funcInst);
}
