#ifndef WASMVM_LOADER_DEF
#define WASMVM_LOADER_DEF

#include <LoaderRequest.h>


typedef struct Loader_ * Loader;

Loader new_Loader();
void free_Loader(Loader loader);
void loader_addRequest(Loader loader, LoaderRequest request);
void loader_activate(Loader loader);
int loader_join(Loader loader, int** resultPtr);

#endif