#include <core/Store.h>

#include <stdlib.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

Store* new_Store()
{
    Store* newStore = (Store*) malloc(sizeof(Store));
    newStore->funcs = new_vector(sizeof(FuncInst), (void(*)(void*))clean_FuncInst);
    newStore->tables = new_vector(sizeof(TableInst), (void(*)(void*))clean_TableInst);
    newStore->mems = new_vector(sizeof(MemInst), (void(*)(void*))clean_MemInst);
    newStore->globals = new_vector(sizeof(GlobalInst), NULL);
    return newStore;
}

void clean_Store(Store* thisStore)
{
    free_vector(thisStore->funcs);
    free_vector(thisStore->tables);
    free_vector(thisStore->mems);
    free_vector(thisStore->globals);
}

void free_Store(Store* thisStore)
{
    clean_Store(thisStore);
    free(thisStore);
}
