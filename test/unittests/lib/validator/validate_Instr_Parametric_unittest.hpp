#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Parametric.h>
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

SKYPAT_F(Validate_Instr_drop, valid)
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

    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = Op_drop;

    // Check
    ValueType* value = (ValueType*)malloc(sizeof(ValueType));
    *value = Value_i32;
    stack_push(opds, value);
    EXPECT_EQ(validate_Instr_drop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_drop, no_enough_operand)
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

    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = Op_drop;

    // Check
    EXPECT_EQ(validate_Instr_drop(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_select, valid)
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

    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = Op_select;

    // Check
    ValueType* value1 = (ValueType*)malloc(sizeof(ValueType));
    *value1 = Value_i64;
    stack_push(opds, value1);
    ValueType* value2 = (ValueType*)malloc(sizeof(ValueType));
    *value2 = Value_i64;
    stack_push(opds, value2);
    ValueType* condition = (ValueType*)malloc(sizeof(ValueType));
    *condition = Value_i32;
    stack_push(opds, condition);
    EXPECT_EQ(validate_Instr_select(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, Value_i64);

    // Clean
    free(result);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_select, no_enough_operand)
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

    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = Op_select;

    // Check
    EXPECT_EQ(validate_Instr_select(instr, context, opds, ctrls), -1);
    ValueType* value1 = (ValueType*)malloc(sizeof(ValueType));
    *value1 = Value_i64;
    stack_push(opds, value1);
    EXPECT_EQ(validate_Instr_select(instr, context, opds, ctrls), -1);

    ValueType* condition = (ValueType*)malloc(sizeof(ValueType));
    *condition = Value_i32;
    stack_push(opds, condition);
    EXPECT_EQ(validate_Instr_select(instr, context, opds, ctrls), -2);

    ValueType* value2 = (ValueType*)malloc(sizeof(ValueType));
    *value2 = Value_i64;
    stack_push(opds, value2);
    condition = (ValueType*)malloc(sizeof(ValueType));
    *condition = Value_i32;
    stack_push(opds, condition);
    EXPECT_EQ(validate_Instr_select(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}