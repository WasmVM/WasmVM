#include <core/Store.h>

#include <stdlib.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

Store* new_Store()
{
    Store* newStore = (Store*) malloc(sizeof(Store));
    newStore->funcs = new_vector_p(sizeof(FuncInst), (void(*)(void*))clean_FuncInst);
    newStore->tables = new_vector_p(sizeof(TableInst), (void(*)(void*))clean_TableInst);
    newStore->mems = new_vector_p(sizeof(MemInst), (void(*)(void*))clean_MemInst);
    newStore->globals = new_vector_p(sizeof(GlobalInst), NULL);
    return newStore;
}

void clean_Store(Store* thisStore)
{
    free_vector_p(thisStore->funcs);
    free_vector_p(thisStore->tables);
    free_vector_p(thisStore->mems);
    free_vector_p(thisStore->globals);
}

void free_Store(Store* thisStore)
{
    clean_Store(thisStore);
    free(thisStore);
}
