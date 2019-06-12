#include "Executor_.h"
#include <core/Core_.h>

#include <stdlib.h>

int executor_run(Executor executor)
{
    if(executor->status != Executor_Stop) {
        return -1;
    }
    for(uint32_t i = 0; i < vector_size(executor->cores); ++i) {
        Core core = vector_at(Core, executor->cores, i);
        int res = core_run(core);
        if(res) {
            return res;
        }
    }
    executor->status = Executor_Running;
    return 0;
}
int executor_stop(Executor executor)
{
    if(executor->status != Executor_Running) {
        return -1;
    }
    executor->status = Executor_Terminated;
    for(uint32_t i = 0; i < vector_size(executor->cores); ++i) {
        Core core = vector_at(Core, executor->cores, i);
        int res = core_stop(core);
        if(res) {
            return res;
        }
    }
    executor->status = Executor_Stop;
    return 0;
}

int executor_join(Executor executor)
{
    if(executor->status != Executor_Running) {
        return 0;
    }
    while(atomic_load(&(executor->runningCores)) > 0) {
        pthread_mutex_lock(&(executor->mutex));
        pthread_cond_wait(&(executor->cond), &(executor->mutex));
        pthread_mutex_unlock(&(executor->mutex));
    }
    executor->status = Executor_Stop;
    return 0;
}
int executor_addModule(Executor executor, ModuleInst* module, uint32_t startFuncIndex)
{
    if(executor->status == Executor_Terminated) {
        return -1;
    }
    vector_push_back(executor->modules, module);
    Core core = new_Core(executor, module, *vector_at(uint32_t*, module->funcaddrs, startFuncIndex));
    vector_push_back(executor->cores, core);
    if(executor->status == Executor_Running) {
        return core_run(core);
    }
    return 0;
}

vector_p executor_get_modules(Executor executor)
{
    return executor->modules;
}
Store executor_get_store(Executor executor)
{
    return executor->store;
}

Executor new_Executor()
{
    Executor executor = (Executor) malloc(sizeof(struct Executor_));
    atomic_init(&(executor->runningCores), 0);
    pthread_mutex_init(&(executor->mutex), NULL);
    pthread_cond_init(&(executor->cond), NULL);
    executor->status = Executor_Stop;
    executor->cores = new_vector_p(struct Core_, clean_Core);
    executor->modules = new_vector_p(ModuleInst, clean_ModuleInst);
    executor->store = new_Store();
    return executor;
}
void free_Executor(Executor executor)
{
    free_vector_p(executor->cores);
    free_vector_p(executor->modules);
    free_Store(executor->store);
    free(executor);
}