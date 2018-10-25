#include <instance/FuncInst.h>

#include <stdlib.h>
#include <dataTypes/Value.h>
#include <instance/InstrInst.h>

FuncInst* new_FuncInst(){
    FuncInst* instance = (FuncInst*) malloc(sizeof(FuncInst));
    instance->type.params = new_vector(sizeof(ValueType), free);
    instance->type.results = new_vector(sizeof(ValueType), free);
    instance->locals = new_vector(sizeof(ValueType), free);
    instance->module = NULL;
    instance->code = new_vector(sizeof(InstrInst), (void(*)(void*))free_InstrInst);
    instance->hostcode = NULL;
    return instance;
}

void free_FuncInst(FuncInst* funcInst){
    free_vector(funcInst->locals);
    free_vector(funcInst->code);
}