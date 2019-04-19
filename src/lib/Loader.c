#include <Loader.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>

#include <Stage.h>

static void addRequest(struct Loader_* loader, LoaderRequest* request)
{
    // Check whether loaded or not
    _Bool loaded = 0;
    for(listNode* cur = loader->loadedList->head; cur != NULL; cur = cur->next) {
        if(!strcmp(request->moduleName, (char*)cur->data)) {
            loaded = 1;
            break;
        }
    }
    if(loaded) {
        request->parent.free((Request*)request);
        return;
    }
    char* loadName = (char*)malloc(sizeof(char) * strlen(request->moduleName));
    strcpy(loadName, request->moduleName);
    loader->loadedList->push_back(loader->loadedList, loadName);
    loader->requests->push(loader->requests, request);
}

static void* run_Loader(Loader* loader)
{
    int* result = (int*) malloc(sizeof(int));
    *result = 0;
    // Run requests
    while(loader->requests->size > 0 && !loader->parent.isTerminated) {
        LoaderRequest* request = NULL;
        loader->requests->pop(loader->requests, (void**)&request);
        // Run stages
        while(request->parent.stages->size > 1) {
            Stage* stage = NULL;
            request->parent.stages->pop(request->parent.stages, (void**)&stage);
            *result = stage->run(stage);
            free(stage);
            if(*result) {
                request->parent.free((Request*)request);
                return result;
            }
        }
        loader->decodedStack->push(loader->decodedStack, request);
    }
    // Run instanciator
    while(loader->decodedStack->size > 0 && !loader->parent.isTerminated) {
        LoaderRequest* request = NULL;
        loader->decodedStack->pop(loader->decodedStack, (void**)&request);
        Stage* stage = NULL;
        request->parent.stages->pop(request->parent.stages, (void**)&stage);
        *result = stage->run(stage);
        request->parent.free((Request*)request);
        free(stage);
        if(*result) {
            return result;
        }
    }
    return result;
}

static int activate_Loader(Component* component)
{
    return pthread_create(&component->thread, NULL, (void* (*) (void*))run_Loader, (void*)component);
}

static void terminate_Loader(Component* component)
{
    component->isTerminated = 1;
}

static int join_Loader(Component* component, int** resultPtr)
{
    return pthread_join(component->thread, (void**)resultPtr);
}

static void freeRequest(Request* request)
{
    request->free(request);
}

Loader* new_Loader()
{
    Loader* newLoader = (Loader*)malloc(sizeof(Loader));
    newLoader->parent.activate = activate_Loader;
    newLoader->parent.terminate = terminate_Loader;
    newLoader->parent.join = join_Loader;
    newLoader->parent.isTerminated = 0;
    newLoader->addRequest = addRequest;
    newLoader->loadedList = new_list(free);
    newLoader->decodedStack = new_stack((void (*)(void*))freeRequest);
    newLoader->requests = new_queue((void (*)(void*))freeRequest);
    return newLoader;
}

void free_Loader(Loader* loader)
{
    free_list(loader->loadedList);
    free_stack(loader->decodedStack);
    free_queue(loader->requests);
    free(loader);
}
