#include <dataTypes/FuncType.h>

#include <stdlib.h>
#include <dataTypes/Value.h>

FuncType new_FuncType()
{
    FuncType newFuncType = (FuncType)malloc(sizeof(struct FuncType_));
    newFuncType->params = new_vector_p(ValueType, NULL);
    newFuncType->results = new_vector_p(ValueType, NULL);
    return newFuncType;
}

void free_FuncType(FuncType funcType)
{
    clean_FuncType(funcType);
    free(funcType);
}
void clean_FuncType(FuncType funcType)
{
    free_vector_p(funcType->params);
    free_vector_p(funcType->results);
}
