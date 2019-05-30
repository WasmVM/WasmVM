#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include <Executor.h>
#include <dataTypes/FuncType.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
}
#undef _Bool

SKYPAT_F(Executor, create_delete)
{
    // Prepare
    Executor* executor = new_Executor();

    // Check
    EXPECT_EQ(atomic_load(executor->runningCores), 0);
    EXPECT_EQ(executor->status, Executor_Stop);
    EXPECT_EQ(executor->cores->length, 0);
    EXPECT_EQ(executor->modules->length, 0);
    EXPECT_NE(executor->store, NULL);

    free_Executor(executor);
}

SKYPAT_F(Executor, add_module)
{
    // Prepare
    Executor* executor = new_Executor();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t funcAddr = 0;
    module->funcaddrs->push_back(module->funcaddrs, &funcAddr);
    FuncType* type = new_FuncType();
    FuncInst* func = new_FuncInst(module, type);
    module->types->push_back(module->types, type);
    executor->store->funcs->push_back(executor->store->funcs, func);

    // Check
    EXPECT_EQ(executor->addModule(executor, module, 0), 0);
    EXPECT_EQ(executor->cores->length, 1);

    free_Executor(executor);
}

SKYPAT_F(Executor, add_module_terminated)
{
    // Prepare
    Executor* executor = new_Executor();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t funcAddr = 0;
    module->funcaddrs->push_back(module->funcaddrs, &funcAddr);
    FuncType* type = new_FuncType();
    FuncInst* func = new_FuncInst(module, type);
    module->types->push_back(module->types, type);
    executor->store->funcs->push_back(executor->store->funcs, func);

    // Check
    EXPECT_EQ(executor->addModule(executor, module, 0), -1);

    free_Executor(executor);
}
