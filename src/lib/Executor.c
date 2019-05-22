#include <Executor.h>

#include <stdlib.h>

static int run_Executor(Executor* executor)
{
    return 0;
}

static int stop_Executor(Executor* executor)
{
    return 0;
}

static int addModule_Executor(Executor* executor, ModuleInst* module, uint32_t startFuncIndex)
{
    if(executor->status == Executor_Terminated) {
        return -1;
    }
    executor->modules->push_back(executor->modules, module);
    Core* core = new_Core(executor->store, module, *(uint32_t*)module->funcaddrs->at(module->funcaddrs, startFuncIndex));
    executor->cores->push_back(executor->cores, (void*) core);
    if(executor->status == Executor_Running) {
        return core->run(core);
    }
    return 0;
}

Executor* new_Executor()
{
    Executor* executor = (Executor*) malloc(sizeof(Executor));
    executor->status = Executor_Stop;
    executor->run = run_Executor;
    executor->stop = stop_Executor;
    executor->cores = new_vector(sizeof(Core), (void(*)(void*))clean_Core);
    executor->modules = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    executor->store = new_Store();
    executor->addModule = addModule_Executor;
    return executor;
}
void free_Executor(Executor* executor)
{
    free_vector(executor->cores);
    free_vector(executor->modules);
    free_Store(executor->store);
    free(executor);
}