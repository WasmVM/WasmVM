#include <dataTypes/FuncType.h>

void free_FuncType(FuncType* funcType){
    free_vector(funcType->params);
    free_vector(funcType->results);
    free(funcType);
}