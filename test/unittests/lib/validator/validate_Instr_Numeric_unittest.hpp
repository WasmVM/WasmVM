#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack_p.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Numeric.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void test_unop(stack_p opds, Context* context, stack_p ctrls, WasmNumericInstr* instr, ValueType expect)
{
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    stack_push(opds, value);
    *value = expect;
    EXPECT_EQ(validate_Instr_unop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, expect);
    free(result);
}

static void test_binop(stack_p opds, Context* context, stack_p ctrls, WasmNumericInstr* instr, ValueType expect)
{
    ValueType* value1 = (ValueType*) malloc(sizeof(ValueType));
    *value1 = expect;
    stack_push(opds, value1);
    ValueType* value2 = (ValueType*) malloc(sizeof(ValueType));
    *value2 = expect;
    stack_push(opds, value2);

    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, expect);
    free(result);
}

static void test_cvtop(stack_p opds, Context* context, stack_p ctrls, WasmNumericInstr* instr, ValueType expect, ValueType result)
{
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    *value = expect;
    stack_push(opds, value);

    EXPECT_EQ(validate_Instr_cvtop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* resultVal = stack_pop(ValueType*, opds);
    EXPECT_EQ(*resultVal, result);
    free(resultVal);
}

static void test_relop(stack_p opds, Context* context, stack_p ctrls, WasmNumericInstr* instr, ValueType input)
{
    ValueType* value1 = (ValueType*) malloc(sizeof(ValueType));
    *value1 = input;
    stack_push(opds, value1);
    ValueType* value2 = (ValueType*) malloc(sizeof(ValueType));
    *value2 = input;
    stack_push(opds, value2);

    EXPECT_EQ(validate_Instr_relop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, Value_i32);
    free(result);
}

static void clean(stack_p opds, stack_p ctrls)
{
    free_stack_p(opds);
    free_stack_p(ctrls);
}

SKYPAT_F(Validate_Instr_const, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 0;

    // Check
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* value = stack_pop(ValueType*, opds);
    EXPECT_EQ(*value, Value_i32);
    free(value);

    instr->constant.type = Value_f32;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    value = stack_pop(ValueType*, opds);
    EXPECT_EQ(*value, Value_f32);
    free(value);

    instr->constant.type = Value_i64;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    value = stack_pop(ValueType*, opds);
    EXPECT_EQ(*value, Value_i64);
    free(value);

    instr->constant.type = Value_f64;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    value = stack_pop(ValueType*, opds);
    EXPECT_EQ(*value, Value_f64);
    free(value);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_unop, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_clz;
    test_unop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_ctz;
    test_unop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_popcnt;
    test_unop(opds, context, ctrls, instr, Value_i32);

    instr->parent.opcode = Op_i64_clz;
    test_unop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_ctz;
    test_unop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_popcnt;
    test_unop(opds, context, ctrls, instr, Value_i64);

    instr->parent.opcode = Op_f32_abs;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_neg;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_ceil;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_floor;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_trunc;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_nearest;
    test_unop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_sqrt;
    test_unop(opds, context, ctrls, instr, Value_f32);

    instr->parent.opcode = Op_f64_abs;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_neg;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_ceil;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_floor;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_trunc;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_nearest;
    test_unop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_sqrt;
    test_unop(opds, context, ctrls, instr, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_unop, no_enough_operand)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_clz;
    EXPECT_EQ(validate_Instr_unop(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_binop, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_add;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_sub;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_mul;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_div_s;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_div_u;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_rem_s;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_rem_u;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_and;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_or;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_xor;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_shl;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_shr_s;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_shr_u;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_rotl;
    test_binop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_rotr;
    test_binop(opds, context, ctrls, instr, Value_i32);

    instr->parent.opcode = Op_i64_add;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_sub;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_mul;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_div_s;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_div_u;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_rem_s;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_rem_u;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_and;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_or;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_xor;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_shl;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_shr_s;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_shr_u;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_rotl;
    test_binop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_rotr;
    test_binop(opds, context, ctrls, instr, Value_i64);

    instr->parent.opcode = Op_f32_add;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_sub;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_mul;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_div;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_min;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_max;
    test_binop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_copysign;
    test_binop(opds, context, ctrls, instr, Value_f32);

    instr->parent.opcode = Op_f64_add;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_sub;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_mul;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_div;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_min;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_max;
    test_binop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_copysign;
    test_binop(opds, context, ctrls, instr, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_binop, no_enough_operand)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_add;
    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), -2);
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    *value = Value_i32;
    stack_push(opds, value);
    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_testop, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    ValueType* value1 = (ValueType*) malloc(sizeof(ValueType));
    stack_push(opds, value1);
    *value1 = Value_i32;
    instr->parent.opcode = Op_i32_eqz;
    EXPECT_EQ(validate_Instr_testop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    ValueType* result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, Value_i32);
    free(result);

    ValueType* value2 = (ValueType*) malloc(sizeof(ValueType));
    stack_push(opds, value2);
    *value2 = Value_i64;
    instr->parent.opcode = Op_i64_eqz;
    EXPECT_EQ(validate_Instr_testop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(stack_size(opds), 1);
    result = stack_pop(ValueType*, opds);
    EXPECT_EQ(*result, Value_i32);
    free(result);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_testop, no_enough_operand)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_eqz;
    EXPECT_EQ(validate_Instr_testop(instr, context, opds, ctrls), -2);

    instr->parent.opcode = Op_i64_eqz;
    EXPECT_EQ(validate_Instr_testop(instr, context, opds, ctrls), -2);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_relop, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_eq;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_ne;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_lt_s;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_lt_u;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_gt_s;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_gt_u;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_le_s;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_le_u;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_ge_s;
    test_relop(opds, context, ctrls, instr, Value_i32);
    instr->parent.opcode = Op_i32_ge_u;
    test_relop(opds, context, ctrls, instr, Value_i32);

    instr->parent.opcode = Op_i64_eq;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_ne;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_lt_s;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_lt_u;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_gt_s;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_gt_u;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_le_s;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_le_u;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_ge_s;
    test_relop(opds, context, ctrls, instr, Value_i64);
    instr->parent.opcode = Op_i64_ge_u;
    test_relop(opds, context, ctrls, instr, Value_i64);

    instr->parent.opcode = Op_f32_eq;
    test_relop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_ne;
    test_relop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_lt;
    test_relop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_gt;
    test_relop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_le;
    test_relop(opds, context, ctrls, instr, Value_f32);
    instr->parent.opcode = Op_f32_ge;
    test_relop(opds, context, ctrls, instr, Value_f32);

    instr->parent.opcode = Op_f64_eq;
    test_relop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_ne;
    test_relop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_lt;
    test_relop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_gt;
    test_relop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_le;
    test_relop(opds, context, ctrls, instr, Value_f64);
    instr->parent.opcode = Op_f64_ge;
    test_relop(opds, context, ctrls, instr, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_relop, no_enough_operand)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_eq;
    EXPECT_EQ(validate_Instr_relop(instr, context, opds, ctrls), -2);
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    *value = Value_i32;
    stack_push(opds, value);
    EXPECT_EQ(validate_Instr_relop(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}

SKYPAT_F(validate_Instr_cvtop, valid)
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

    WasmNumericInstr* instr = new_WasmNumericInstr();

    // Check
    instr->parent.opcode = Op_i32_wrap_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_i32);
    instr->parent.opcode = Op_i32_trunc_s_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_i32);
    instr->parent.opcode = Op_i32_trunc_u_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_i32);
    instr->parent.opcode = Op_i32_trunc_s_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_i32);
    instr->parent.opcode = Op_i32_trunc_u_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_i32);
    instr->parent.opcode = Op_i64_extend_s_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_i64);
    instr->parent.opcode = Op_i64_extend_u_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_i64);
    instr->parent.opcode = Op_i64_trunc_s_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_i64);
    instr->parent.opcode = Op_i64_trunc_u_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_i64);
    instr->parent.opcode = Op_i64_trunc_s_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_i64);
    instr->parent.opcode = Op_i64_trunc_u_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_i64);
    instr->parent.opcode = Op_f32_convert_s_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_f32);
    instr->parent.opcode = Op_f32_convert_u_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_f32);
    instr->parent.opcode = Op_f32_convert_s_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_f32);
    instr->parent.opcode = Op_f32_convert_u_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_f32);
    instr->parent.opcode = Op_f32_demote_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_f32);
    instr->parent.opcode = Op_f64_convert_s_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_f64);
    instr->parent.opcode = Op_f64_convert_u_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_f64);
    instr->parent.opcode = Op_f64_convert_s_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_f64);
    instr->parent.opcode = Op_f64_convert_u_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_f64);
    instr->parent.opcode = Op_f64_promote_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_f64);
    instr->parent.opcode = Op_i32_reinterpret_f32;
    test_cvtop(opds, context, ctrls, instr, Value_f32, Value_i32);
    instr->parent.opcode = Op_i64_reinterpret_f64;
    test_cvtop(opds, context, ctrls, instr, Value_f64, Value_i64);
    instr->parent.opcode = Op_f32_reinterpret_i32;
    test_cvtop(opds, context, ctrls, instr, Value_i32, Value_f32);
    instr->parent.opcode = Op_f64_reinterpret_i64;
    test_cvtop(opds, context, ctrls, instr, Value_i64, Value_f64);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}