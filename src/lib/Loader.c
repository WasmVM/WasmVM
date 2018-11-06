#include <Loader.h>

#include <stdlib.h>
#include <pthread.h>
#include <stddef.h>
#include <Stage.h>

static void addRequest(struct Loader_* loader, Request* request){
    loader->requests->push(loader->requests, request);
}

static int run_Loader(Loader* loader){
    // Run requests
    while(loader->requests->size > 0 && !loader->parent.isTerminated){
        Request* request = NULL;
        loader->requests->pop(loader->requests, (void**)&request);
        while(request->stages->size > 1){
            Stage* stage = NULL;
            request->stages->pop(request->stages, (void**)&stage);
            int result = stage->run();
            if(result){
                return result;
            }
        }
        loader->decodedStack->push(loader->decodedStack, request);
    }
    // Run instanciator
    while(loader->decodedStack->size > 0 && !loader->parent.isTerminated){
        Request* request = NULL;
        loader->decodedStack->pop(loader->decodedStack, (void**)&request);
        Stage* stage = NULL;
        request->stages->pop(request->stages, (void**)&stage);
        int result = stage->run();
        if(result){
            return result;
        }
    }
}

static int activate_Loader(Component* component){
    pthread_create(component->thread, NULL, (void*)run_Loader, (void*)component);
}

static void terminate_Loader(Component* component){
    component->isTerminated = 1;
}

static void freeRequest(Request* request){
    request->free(request);
}

Loader* new_Loader(){
    Loader* newLoader = (Loader*)malloc(sizeof(Loader));
    newLoader->parent.activate = activate_Loader;
    newLoader->parent.terminate = terminate_Loader;
    newLoader->parent.thread = NULL;
    newLoader->parent.isTerminated = 0;
    newLoader->addRequest = addRequest;
    newLoader->loadedList = new_list(free);
    newLoader->decodedStack = new_stack();
    newLoader->requests = new_queue(freeRequest);
    return newLoader;
}
void free_Loader(Loader* loader){
    free_list(loader->loadedList);
    free_stack(loader->decodedStack);
    free_queue(loader->requests);
    free(loader);
}