#include <WasmVM.h>

#include <defines.h>

Store* store_init()
{
    Store* store = (Store*) malloc_func(sizeof(Store));
    vector_init(store->funcs);
    vector_init(store->tables);
    vector_init(store->mems);
    vector_init(store->globals);
    vector_init(store->elems);
    vector_init(store->datas);
    return store;
}

void store_free(Store* store)
{
    // TODO:
}