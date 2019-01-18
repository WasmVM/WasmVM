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
#include <structures/WasmTable.h>
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_block;
    instr->resultTypes = new_vector(sizeof(ValueType));
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
    EXPECT_EQ(frame->label_types->length, 2);
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_loop;
    instr->resultTypes = new_vector(sizeof(ValueType));
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
    EXPECT_EQ(frame->label_types->length, 0);
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType));
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType));
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_if;
    instr->resultTypes = new_vector(sizeof(ValueType));
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
    EXPECT_EQ(frame->label_types->length, 2);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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
    ctrl_frame* frame = new_ctrl_frame(opds);
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

SKYPAT_F(Validate_Instr_br, valid)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    ValueType* labelType1 = (ValueType*)malloc(sizeof(ValueType));
    *labelType1 = Value_i32;
    frame->label_types->push_back(frame->label_types, labelType1);
    frame->end_types->push_back(frame->end_types, labelType1);

    // Check
    EXPECT_EQ(validate_Instr_br(instr, context, opds, ctrls), 0);
    EXPECT_TRUE(frame->unreachable);
    EXPECT_EQ(opds->size, 0);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_br, index_out_of_range)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    ValueType* labelType1 = (ValueType*)malloc(sizeof(ValueType));
    *labelType1 = Value_i32;
    frame->label_types->push_back(frame->label_types, labelType1);
    frame->end_types->push_back(frame->end_types, labelType1);

    // Check
    EXPECT_EQ(validate_Instr_br(instr, context, opds, ctrls), -1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_br, no_enough_operand)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType1 = (ValueType*)malloc(sizeof(ValueType));
    *labelType1 = Value_i32;
    frame->label_types->push_back(frame->label_types, labelType1);
    frame->end_types->push_back(frame->end_types, labelType1);

    // Check
    EXPECT_EQ(validate_Instr_br(instr, context, opds, ctrls), -2);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_br_if, valid)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_if;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    ValueType* labelType1 = (ValueType*)malloc(sizeof(ValueType));
    *labelType1 = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType1);
    frame->end_types->push_back(frame->end_types, labelType1);

    // Check
    EXPECT_EQ(validate_Instr_br_if(instr, context, opds, ctrls), 0);
    EXPECT_FALSE(frame->unreachable);
    EXPECT_EQ(opds->size, 1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_br_if, index_out_of_range)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_if;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_br_if(instr, context, opds, ctrls), -1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_if, no_condition_operand)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_if;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    // Check
    EXPECT_EQ(validate_Instr_br_if(instr, context, opds, ctrls), -2);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Instr_br_if, no_enough_operand)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_if;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices = new_vector(sizeof(uint32_t));
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    ValueType* labelType1 = (ValueType*)malloc(sizeof(ValueType));
    *labelType1 = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType1);
    frame->end_types->push_back(frame->end_types, labelType1);

    // Check
    EXPECT_EQ(validate_Instr_br_if(instr, context, opds, ctrls), -3);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, valid)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), 0);
    EXPECT_TRUE(frame2->unreachable);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, index_out_of_range)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 5;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, index_in_table_out_of_range)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 5;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -2);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, other_frame_no_enough_label)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f64;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -3);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, other_frame_wrong_type_label)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f64;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);
    ValueType* opdType2 = (ValueType*)malloc(sizeof(ValueType));
    *opdType2 = Value_i32;
    opds->push(opds, opdType2);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -4);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, no_enough_operand)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_f32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -5);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_br_table, no_enough_label)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_br_table;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);
    index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame->label_types->push_back(frame->label_types, labelType);
    frame->end_types->push_back(frame->end_types, labelType);
    ctrl_frame* frame1 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame1);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame1->label_types->push_back(frame1->label_types, labelType);
    frame1->end_types->push_back(frame1->end_types, labelType);
    ctrl_frame* frame2 = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame2);
    labelType = (ValueType*)malloc(sizeof(ValueType));
    *labelType = Value_f32;
    frame2->label_types->push_back(frame2->label_types, labelType);
    frame2->end_types->push_back(frame2->end_types, labelType);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_br_table(instr, context, opds, ctrls), -6);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_return, valid)
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
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_return;

    // Check
    EXPECT_EQ(validate_Instr_return(instr, context, opds, ctrls), 0);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    WasmFunc* func2 = new_WasmFunc();
    func2->type = 0;
    module->funcs->push_back(module->funcs, func2);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);

    // Check
    EXPECT_EQ(validate_Instr_call(instr, context, opds, ctrls), 0);
    *index = 1;
    EXPECT_EQ(validate_Instr_call(instr, context, opds, ctrls), 0);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call, index_out_of_range)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    WasmFunc* func2 = new_WasmFunc();
    func2->type = 0;
    module->funcs->push_back(module->funcs, func2);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    instr->indices->push_back(instr->indices, index);

    // Check
    EXPECT_EQ(validate_Instr_call(instr, context, opds, ctrls), -1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call_indirect, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    WasmTable* table = (WasmTable*) malloc(sizeof(WasmTable));
    table->min = 0;
    table->max = 1;
    module->tables->push_back(module->tables, table);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call_indirect;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_call_indirect(instr, context, opds, ctrls), 0);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call_indirect, no_table)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call_indirect;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_call_indirect(instr, context, opds, ctrls), -1);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call_indirect, index_out_of_range)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    WasmTable* table = (WasmTable*) malloc(sizeof(WasmTable));
    table->min = 0;
    table->max = 1;
    module->tables->push_back(module->tables, table);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call_indirect;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    instr->indices->push_back(instr->indices, index);

    ValueType* opdType1 = (ValueType*)malloc(sizeof(ValueType));
    *opdType1 = Value_i32;
    opds->push(opds, opdType1);

    // Check
    EXPECT_EQ(validate_Instr_call_indirect(instr, context, opds, ctrls), -2);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_call_indirect, no_enough_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func1 = new_WasmFunc();
    func1->type = 0;
    module->funcs->push_back(module->funcs, func1);
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    WasmTable* table = (WasmTable*) malloc(sizeof(WasmTable));
    table->min = 0;
    table->max = 1;
    module->tables->push_back(module->tables, table);
    Context* context = new_Context(module, func1);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrls->push(ctrls, new_ctrl_frame(opds));

    WasmControlInstr* instr = (WasmControlInstr*)malloc(sizeof(WasmControlInstr));
    instr->parent.opcode = Op_call_indirect;
    instr->indices = new_vector(sizeof(uint32_t));
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    instr->indices->push_back(instr->indices, index);

    // Check
    EXPECT_EQ(validate_Instr_call_indirect(instr, context, opds, ctrls), -3);

    // Clean
    free_vector(instr->indices);
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}
