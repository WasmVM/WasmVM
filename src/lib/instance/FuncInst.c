#include <instance/FuncInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

FuncInst* new_FuncInst(ModuleInst* module, FuncType* type)
{
    FuncInst* instance = (FuncInst*) malloc(sizeof(FuncInst));
    instance->type = type;
    instance->locals = new_vector(sizeof(ValueType), NULL);
    instance->module = module;
    instance->code = new_list((void(*)(void*))free_InstrInst);
    instance->hostcode = NULL;
    return instance;
}

void clean_FuncInst(FuncInst* funcInst)
{
    free_vector(funcInst->locals);
    free_list(funcInst->code);
}

void free_FuncInst(FuncInst* funcInst)
{
    clean_FuncInst(funcInst);
    free(funcInst);
}
