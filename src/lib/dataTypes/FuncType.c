#include <dataTypes/FuncType.h>

#include <stdlib.h>

void free_FuncType(FuncType* funcType){
    free_vector(funcType->params);
    free_vector(funcType->results);
    free(funcType);
}