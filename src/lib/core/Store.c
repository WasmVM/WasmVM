#include <core/Store.h>

#include <stdlib.h>
#include <instance/FuncInst.h>
#include <instance/TableInst.h>
#include <instance/MemInst.h>
#include <instance/GlobalInst.h>

Store new_Store()
{
    Store newStore = (Store) malloc(sizeof(struct Store_));
    newStore->funcs = new_vector_p(FuncInst, clean_FuncInst);
    newStore->tables = new_vector_p(TableInst, clean_TableInst);
    newStore->mems = new_vector_p(MemInst, clean_MemInst);
    newStore->globals = new_vector_p(GlobalInst, NULL);
    return newStore;
}

void clean_Store(Store thisStore)
{
    free_vector_p(thisStore->funcs);
    free_vector_p(thisStore->tables);
    free_vector_p(thisStore->mems);
    free_vector_p(thisStore->globals);
}

void free_Store(Store thisStore)
{
    clean_Store(thisStore);
    free(thisStore);
}
