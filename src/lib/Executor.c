#include <Executor.h>

#include <stdlib.h>
#include <instance/ModuleInst.h>

static void* run_Executor(Executor* executor)
{
    int* result = (int*) malloc(sizeof(int));
    *result = 0;
    // TODO:
    return result;
}

static int activate_Executor(Component* component)
{
    return pthread_create(&component->thread, NULL, (void* (*) (void*))run_Executor, (void*)component);
}

static void terminate_Executor(Component* component)
{
    component->isTerminated = 1;
}

static int join_Executor(Component* component, int** resultPtr)
{
    return pthread_join(component->thread, (void**)resultPtr);
}

Executor* new_Executor()
{
    Executor* executor = (Executor*) malloc(sizeof(Executor));
    executor->parent.activate = activate_Executor;
    executor->parent.terminate = terminate_Executor;
    executor->parent.isTerminated = 0;
    executor->parent.join = join_Executor;
    executor->cores = NULL; // FIXME:
    executor->modules = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    executor->store = new_Store();
    return executor;
}
void free_Executor(Executor* executor)
{
    free_vector(executor->modules);
    free_Store(executor->store);
    free(executor);
}