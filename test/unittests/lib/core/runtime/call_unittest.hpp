#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <string.h>
#include <stdlib.h>
#include <core/Runtime.h>
#include <core/Stack.h>
#include <core/Store.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/FuncType.h>
#include <instance/ControlInstrInst.h>
#include <instance/FuncInst.h>
#include <instance/ModuleInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_control_if, regular)
{
    Stack* stack = new_Stack();
    Store* store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    uint32_t* funcAddr = (uint32_t*) malloc(sizeof(uint32_t));
    *funcAddr = 0;
    module->funcaddrs->push_back(module->funcaddrs, funcAddr);
    FuncType type = new_FuncType();
    ValueType* param = (ValueType*) malloc(sizeof(ValueType));
    *param = Value_i32;
    type->params->push_back(type->params, param);
    FuncInst* func = new_FuncInst(module, type);
    ValueType* local = (ValueType*) malloc(sizeof(ValueType));
    *local = Value_f32;
    func->locals->push_back(func->locals, local);
    store->funcs->push_back(store->funcs, (void*)func);
    Frame frame = new_Frame(module);
    push_Frame(stack, frame);

    ControlInstrInst* control = new_ControlInstrInst();
    uint32_t* address = (uint32_t*) malloc(sizeof(uint32_t));
    *address = 0;
    control->indices->push_back(control->indices, address);
    // Check
    push_Value(stack, new_i32Value(28));
    EXPECT_EQ(runtime_call(stack, store, control), 0);
    Label resultLabel = NULL;
    EXPECT_EQ(pop_Label(stack, &resultLabel), 0);
    EXPECT_EQ(label_get_funcAddr(resultLabel), 0);
    EXPECT_EQ(label_get_instrIndex(resultLabel), 0);
    EXPECT_EQ(label_get_contInstr(resultLabel), 0);
    EXPECT_EQ(label_get_resultTypes(resultLabel)->length, 0);
    free_Label(resultLabel);
    Frame resultFrame = NULL;
    EXPECT_EQ(pop_Frame(stack, &resultFrame), 0);
    ModuleInst* resultModule = frame_get_module(resultFrame);
    EXPECT_EQ(resultModule, module);
    vector_p resultLocals = frame_get_locals(resultFrame);
    EXPECT_EQ(resultLocals->length, 2);
    Value* resultLocal = (Value*)resultLocals->at(resultLocals, 0);
    EXPECT_EQ(resultLocal->type, Value_i32);
    EXPECT_EQ(resultLocal->value.i32, 28);
    resultLocal = (Value*)resultLocals->at(resultLocals, 1);
    EXPECT_EQ(resultLocal->type, Value_f32);
    EXPECT_EQ(resultLocal->value.f32, 0);
    free_Frame(resultFrame);

    // clean
    free_Stack(stack);
    free_Store(store);
    free_ModuleInst(module);
    free_ControlInstrInst(control);
}
