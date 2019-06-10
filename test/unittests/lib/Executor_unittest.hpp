#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Executor.h>
#include <dataTypes/FuncType.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include "../util/ExecutorHelper.h"
}
#undef _Bool

SKYPAT_F(Executor, create_delete)
{
    // Prepare
    Executor executor = new_Executor();

    // Check
    EXPECT_EQ(executor_get_runningCores(executor), 0);
    EXPECT_EQ(vector_size(executor_get_cores(executor)), 0);
    EXPECT_EQ(vector_size(executor_get_modules(executor)), 0);
    EXPECT_NE(executor_get_store(executor->store), NULL);

    free_Executor(executor);
}

SKYPAT_F(Executor, add_module)
{
    // Prepare
    Executor executor = new_Executor();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t funcAddr = 0;
    vector_push_back(module->funcaddrs, &funcAddr);
    FuncType type = new_FuncType();
    FuncInst* func = new_FuncInst(module, type);
    vector_push_back(module->types, type);
    vector_push_back(executor_get_store(executor)->funcs, func);

    // Check
    EXPECT_EQ(executor_addModule(executor, module, 0), 0);
    EXPECT_EQ(vector_size(executor->cores), 1);

    free_Executor(executor);
}

SKYPAT_F(Executor, add_module_terminated)
{
    // Prepare
    Executor executor = new_Executor();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t funcAddr = 0;
    vector_push_back(module->funcaddrs, &funcAddr);
    FuncType type = new_FuncType();
    FuncInst* func = new_FuncInst(module, type);
    vector_push_back(module->types, type);
    vector_push_back(executor_get_store(executor)->funcs, func);

    // Check
    EXPECT_EQ(executor_addModule(executor, module, 0), -1);

    free_Executor(executor);
}
