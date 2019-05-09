#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Opcodes.h>
#include <core/Core.h>
#include <core/Store.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/NumericInstrInst.h>
#include <dataTypes/Value.h>
#include <dataTypes/FuncType.h>
}
#undef _Bool

SKYPAT_F(Core, create_delete)
{
    // Prepare
    Store* store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType* funcType = new_FuncType();
    module->types->push_back(module->types, funcType);
    uint32_t funcAddr = 0;
    module->funcaddrs->push_back(module->funcaddrs, &funcAddr);
    FuncInst* func = new_FuncInst(module, funcType);
    NumericInstrInst* instr1 = new_NumericInstrInst();
    instr1->parent.opcode = Op_i32_const;
    instr1->constant.parent.entryType = Entry_Value;
    instr1->constant.type = Value_i32;
    instr1->constant.value.i32 = 5;
    func->code->push_back(func->code, instr1);
    ValueType localType1 = Value_i32;
    func->locals->push_back(func->locals, &localType1);
    store->funcs->push_back(store->funcs, func);

    // Check
    Core* core = new_Core(store, module, 0);
    EXPECT_EQ(core->stack, NULL);
    EXPECT_NE(core->store, NULL);
    EXPECT_EQ(core->startFuncAddr, 0);
    EXPECT_EQ(core->status, Core_Stop);
    EXPECT_EQ(core->module, module);

    // Clean
    free_Core(core);
    free_Store(store);
    free_ModuleInst(module);
}

SKYPAT_F(Core, run_valid)
{
    // Prepare
    Store* store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType* funcType = new_FuncType();
    module->types->push_back(module->types, funcType);
    uint32_t funcAddr = 0;
    module->funcaddrs->push_back(module->funcaddrs, &funcAddr);
    FuncInst* func = new_FuncInst(module, funcType);
    NumericInstrInst* instr1 = new_NumericInstrInst();
    instr1->parent.opcode = Op_i32_const;
    instr1->constant.parent.entryType = Entry_Value;
    instr1->constant.type = Value_i32;
    instr1->constant.value.i32 = 5;
    func->code->push_back(func->code, instr1);
    NumericInstrInst* instr2 = new_NumericInstrInst();
    instr2->parent.opcode = Op_i32_const;
    instr2->constant.parent.entryType = Entry_Value;
    instr2->constant.type = Value_i32;
    instr2->constant.value.i32 = 7;
    func->code->push_back(func->code, instr2);
    NumericInstrInst* instr3 = new_NumericInstrInst();
    instr3->parent.opcode = Op_i32_add;
    func->code->push_back(func->code, instr3);
    ValueType localType1 = Value_i32;
    func->locals->push_back(func->locals, &localType1);
    store->funcs->push_back(store->funcs, func);

    // Check
    char buf[100];
    fflush(stdout);
    int oldOut = dup(STDOUT_FILENO);
    memset(buf, 0, sizeof(char) * 100);
    freopen("/dev/null", "a", stdout);
    setvbuf(stdout, buf, _IOFBF, 100);

    Core* core = new_Core(store, module, 0);
    EXPECT_EQ(core->run(core), 0);
    EXPECT_EQ(core->stop(core), 0);
    EXPECT_FALSE(strcmp(buf, "[0] i32 12\n"));

    freopen("/dev/null", "a", stdout);
    dup2(oldOut, STDOUT_FILENO);
    setvbuf(stdout, NULL, _IONBF, 100);

    // Clean
    free_Core(core);
    free_Store(store);
    free_ModuleInst(module);
}