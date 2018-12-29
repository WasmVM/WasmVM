#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Control.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void clean(stack* opds, stack* ctrls)
{
    for(stackNode* cur = opds->head; cur != NULL; cur = cur->next) {
        free(cur->data);
    }
    free_stack(opds);
    for(stackNode* cur = ctrls->head; cur != NULL; cur = cur->next) {
        free_ctrl_frame((ctrl_frame*)cur->data);
    }
    free_stack(ctrls);
}

SKYPAT_F(Validate_Instr_nop, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_nop;

    // Check
    EXPECT_EQ(validate_Instr_nop(instr, context, opds, ctrls), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_block, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame());

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_block;
    instr->resultTypes = new_vector(sizeof(ValueType), free);
    ValueType* resType1 = (ValueType*)malloc(sizeof(ValueType));
    *resType1 = Value_i32;
    instr->resultTypes->push_back(instr->resultTypes, resType1);
    ValueType* resType2 = (ValueType*)malloc(sizeof(ValueType));
    *resType2 = Value_i64;
    instr->resultTypes->push_back(instr->resultTypes, resType2);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_block(instr, context, opds, ctrls), 0);
    EXPECT_EQ(ctrls->size, 2);
    ctrl_frame* frame = NULL;
    ctrls->top(ctrls, (void**)&frame);
    EXPECT_EQ(frame->label_types->length, 1);
    EXPECT_EQ(frame->end_types->length, 2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_loop, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame());

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_loop;
    instr->resultTypes = new_vector(sizeof(ValueType), free);
    ValueType* resType1 = (ValueType*)malloc(sizeof(ValueType));
    *resType1 = Value_i32;
    instr->resultTypes->push_back(instr->resultTypes, resType1);
    ValueType* resType2 = (ValueType*)malloc(sizeof(ValueType));
    *resType2 = Value_i64;
    instr->resultTypes->push_back(instr->resultTypes, resType2);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_loop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(ctrls->size, 2);
    ctrl_frame* frame = NULL;
    ctrls->top(ctrls, (void**)&frame);
    EXPECT_EQ(frame->label_types->length, 1);
    EXPECT_EQ(frame->end_types->length, 2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_if, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame());

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType), free);
    ValueType* resType1 = (ValueType*)malloc(sizeof(ValueType));
    *resType1 = Value_i32;
    instr->resultTypes->push_back(instr->resultTypes, resType1);
    ValueType* resType2 = (ValueType*)malloc(sizeof(ValueType));
    *resType2 = Value_i64;
    instr->resultTypes->push_back(instr->resultTypes, resType2);

    // Check
    EXPECT_EQ(validate_Instr_if(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_if, wrong_type_of_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame());

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType), free);
    ValueType* resType1 = (ValueType*)malloc(sizeof(ValueType));
    *resType1 = Value_i32;
    instr->resultTypes->push_back(instr->resultTypes, resType1);
    ValueType* resType2 = (ValueType*)malloc(sizeof(ValueType));
    *resType2 = Value_i64;
    instr->resultTypes->push_back(instr->resultTypes, resType2);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_f32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_if(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_if, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame());

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType), free);
    ValueType* resType1 = (ValueType*)malloc(sizeof(ValueType));
    *resType1 = Value_i32;
    instr->resultTypes->push_back(instr->resultTypes, resType1);
    ValueType* resType2 = (ValueType*)malloc(sizeof(ValueType));
    *resType2 = Value_i64;
    instr->resultTypes->push_back(instr->resultTypes, resType2);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_if(instr, context, opds, ctrls), 0);
    EXPECT_EQ(ctrls->size, 2);
    ctrl_frame* frame = NULL;
    ctrls->top(ctrls, (void**)&frame);
    EXPECT_EQ(frame->label_types->length, 0);
    EXPECT_EQ(frame->end_types->length, 2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_end, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ValueType* endType2 = (ValueType*)malloc(sizeof(ValueType));
    *endType2 = Value_f32;
    frame->end_types->push_back(frame->end_types, endType2);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_end;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_f32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_end(instr, context, opds, ctrls), 0);
    EXPECT_EQ(ctrls->size, 0);
    EXPECT_EQ(opds->size, 2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_end, no_frame_to_end)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_end;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_f32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_end(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_end, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ValueType* endType2 = (ValueType*)malloc(sizeof(ValueType));
    *endType2 = Value_f32;
    frame->end_types->push_back(frame->end_types, endType2);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_end;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_end(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_end, too_much_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_end;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_end(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_else, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ValueType* endType2 = (ValueType*)malloc(sizeof(ValueType));
    *endType2 = Value_f32;
    frame->end_types->push_back(frame->end_types, endType2);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_else;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_f32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_else(instr, context, opds, ctrls), 0);
    EXPECT_EQ(ctrls->size, 1);
    EXPECT_EQ(opds->size, 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_else, no_frame_to_end)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_else;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_f32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_else(instr, context, opds, ctrls), -1);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_else, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ValueType* endType2 = (ValueType*)malloc(sizeof(ValueType));
    *endType2 = Value_f32;
    frame->end_types->push_back(frame->end_types, endType2);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_else;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_else(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_else, too_much_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame();
    ValueType* endType1 = (ValueType*)malloc(sizeof(ValueType));
    *endType1 = Value_i32;
    frame->end_types->push_back(frame->end_types, endType1);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_else;

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_end(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}