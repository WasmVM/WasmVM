#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <Executor.h>
#include <core/Core.h>
#include <instance/ModuleInst.h>
#include <instance/FuncInst.h>
#include <instance/NumericInstrInst.h>
#include <dataTypes/Value.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
}
#undef _Bool

SKYPAT_F(Core, create_delete)
{
    // Prepare
    Executor* executor = new_Executor();
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
    executor->store->funcs->push_back(executor->store->funcs, func);

    // Check
    Core* core = new_Core(executor, module, 0);
    EXPECT_EQ(core->stack, NULL);
    EXPECT_EQ(core->executor, executor);
    EXPECT_EQ(core->startFuncAddr, 0);
    EXPECT_EQ(core->status, Core_Stop);
    EXPECT_EQ(core->module, module);

    // Clean
    free_Core(core);
    free_Executor(executor);
    free_ModuleInst(module);
}

SKYPAT_F(Core, run_stop)
{
    // Prepare
    Executor* executor = new_Executor();
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
    executor->store->funcs->push_back(executor->store->funcs, func);

    // Check
    Core* core = new_Core(executor, module, 0);
    EXPECT_EQ(core->run(core), 0);
    EXPECT_EQ(core->stop(core), 0);

    // Clean
    free_Core(core);
    free_Executor(executor);
    free_ModuleInst(module);
}

SKYPAT_F(Core, resume)
{
    // Prepare
    Executor* executor = new_Executor();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType* funcType = new_FuncType();
    ValueType result1 = Value_i32;
    funcType->results->push_back(funcType->results, &result1);
    ValueType result2 = Value_i32;
    funcType->results->push_back(funcType->results, &result2);
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
    executor->store->funcs->push_back(executor->store->funcs, func);


    Core* core = new_Core(executor, module, 0);
    core->stack = new_Stack();
    FuncInst* startFunc = (FuncInst*)core->executor->store->funcs->at(core->executor->store->funcs, core->startFuncAddr);
    Frame* frame = new_Frame(startFunc->module);
    push_Frame(core->stack, frame);
    Label* label = new_Label(core->startFuncAddr, 0, startFunc->code->size);
    label->resultTypes = startFunc->type->results;
    push_Label(core->stack, label);

    // Check
    EXPECT_EQ(core->resume(core), 0);
    core->stop(core);

    // Clean
    free_Core(core);
    free_Executor(executor);
    free_ModuleInst(module);
}