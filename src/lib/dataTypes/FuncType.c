#include <dataTypes/FuncType.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

FuncType* new_FuncType()
{
    FuncType* newFuncType = (FuncType*)malloc(sizeof(FuncType));
    newFuncType->params = new_vector(sizeof(ValueType));
    newFuncType->results = new_vector(sizeof(ValueType));
    return newFuncType;
}

void free_FuncType(FuncType* funcType)
{
    free_vector(funcType->params);
    free_vector(funcType->results);
    free(funcType);
}
