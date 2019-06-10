#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Variable.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void clean(stack_p opds, stack_p ctrls)
{
    free_stack_p(opds);
    free_stack_p(ctrls);
}

SKYPAT_F(validate_Instr_get_local, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_get_local;

    // Check
    EXPECT_EQ(validate_Instr_get_local(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* operand = stack_top(ValueType*, opds);
    EXPECT_EQ(*operand, Value_i32);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_get_local, local_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_get_local;

    // Check
    EXPECT_EQ(validate_Instr_get_local(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_local, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_local;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_set_local(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_local, local_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_local;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_set_local(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_local, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_local;

    // Check
    EXPECT_EQ(validate_Instr_set_local(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_tee_local, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_tee_local;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_tee_local(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_top(ValueType*, opds);
    EXPECT_EQ(*result, Value_i32);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_tee_local, local_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_get_local;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_tee_local(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_tee_local, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    ValueType* local1 = (ValueType*) malloc(sizeof(ValueType));
    *local1 = Value_i32;
    vector_push_back(func->locals, local1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_tee_local;

    // Check
    EXPECT_EQ(validate_Instr_tee_local(instr, context, opds, ctrls), -2);
    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_get_global, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    WasmGlobal* global1 = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    global1->mut = 1;
    global1->valType = Value_i32;
    global1->init.parent.entryType = Entry_Value;
    global1->init.type = Value_i32;
    global1->init.value.i32 = 1;
    vector_push_back(module->globals, global1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_get_global;

    // Check
    EXPECT_EQ(validate_Instr_get_global(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* operand = stack_top(ValueType*, opds);
    EXPECT_EQ(*operand, Value_i32);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_get_global, global_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_get_global;

    // Check
    EXPECT_EQ(validate_Instr_get_global(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_global, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    WasmGlobal* global1 = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    global1->mut = 1;
    global1->valType = Value_i32;
    global1->init.parent.entryType = Entry_Value;
    global1->init.type = Value_i32;
    global1->init.value.i32 = 1;
    vector_push_back(module->globals, global1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_global;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_set_global(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_global, global_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_global;

    // Check
    ValueType* operand = (ValueType*) malloc(sizeof(ValueType));
    *operand = Value_i32;
    stack_push(opds, operand);
    EXPECT_EQ(validate_Instr_set_global(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_global, mut_is_not_var)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    WasmGlobal* global1 = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    global1->mut = 0;
    global1->valType = Value_i32;
    global1->init.parent.entryType = Entry_Value;
    global1->init.type = Value_i32;
    global1->init.value.i32 = 1;
    vector_push_back(module->globals, global1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_global;

    // Check
    EXPECT_EQ(validate_Instr_set_global(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_set_global, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    WasmGlobal* global1 = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    global1->mut = 1;
    global1->valType = Value_i32;
    global1->init.parent.entryType = Entry_Value;
    global1->init.type = Value_i32;
    global1->init.value.i32 = 1;
    vector_push_back(module->globals, global1);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack_p opds = new_stack_p(free);
    stack_p ctrls = new_stack_p((void(*)(void*))free_ctrl_frame);
    ctrl_frame* frame = new_ctrl_frame(opds);
    stack_push(ctrls, frame);

    WasmVariableInstr* instr = new_WasmVariableInstr(0);
    instr->parent.opcode = Op_set_global;

    // Check
    EXPECT_EQ(validate_Instr_set_global(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}