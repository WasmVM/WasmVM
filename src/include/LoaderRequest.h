#ifndef WASMVM_LOADERREQUEST_DEF
#define WASMVM_LOADERREQUEST_DEF

#include <Request.h>
#include <core/Store.h>
#include <Executor.h>
#include <dataTypes/vector_p.h>
#include <Component.h>

struct LoaderRequest_ {
    Request parent;
    char* moduleName;
};

typedef struct LoaderRequest_ * LoaderRequest;

LoaderRequest new_LoaderRequest_(const char* moduleName, Component* loader, Executor executor);
void free_LoaderRequest(LoaderRequest request);

#define new_LoaderRequest(moduleName, loader, executor) new_LoaderRequest_(moduleName, (Component*)loader, executor)

#endif