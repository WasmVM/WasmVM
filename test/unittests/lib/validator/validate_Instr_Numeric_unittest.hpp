#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/stack.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Numeric.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Validates.h>
#include <Opcodes.h>
#include <Context.h>
}
#undef _Bool

static void test_unop(stack* opds, Context* context, stack* ctrls, WasmNumericInstr* instr, ValueType expect)
{
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    opds->push(opds, value);
    *value = expect;
    EXPECT_EQ(validate_Instr_unop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    ValueType* result = NULL;
    opds->pop(opds, (void**) &result);
    EXPECT_EQ(*result, expect);
    free(result);
}

static void test_binop(stack* opds, Context* context, stack* ctrls, WasmNumericInstr* instr, ValueType expect)
{
    ValueType* value1 = (ValueType*) malloc(sizeof(ValueType));
    *value1 = expect;
    opds->push(opds, value1);
    ValueType* value2 = (ValueType*) malloc(sizeof(ValueType));
    *value1 = expect;
    opds->push(opds, value2);

    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    ValueType* result = NULL;
    opds->pop(opds, (void**) &result);
    EXPECT_EQ(*result, expect);
    free(result);
}

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

SKYPAT_F(Validate_Instr_const, valid)
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

    WasmNumericInstr* instr = (WasmNumericInstr*)malloc(sizeof(WasmNumericInstr));
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 0;

    // Check
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    ValueType* value = NULL;
    opds->pop(opds, (void**) &value);
    EXPECT_EQ(*value, Value_i32);
    free(value);

    instr->constant.type = Value_f32;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    opds->pop(opds, (void**) &value);
    EXPECT_EQ(*value, Value_f32);
    free(value);

    instr->constant.type = Value_i64;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    opds->pop(opds, (void**) &value);
    EXPECT_EQ(*value, Value_i64);
    free(value);

    instr->constant.type = Value_f64;
    EXPECT_EQ(validate_Instr_const(instr, context, opds, ctrls), 0);
    EXPECT_EQ(opds->size, 1);
    opds->pop(opds, (void**) &value);
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
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmNumericInstr* instr = (WasmNumericInstr*)malloc(sizeof(WasmNumericInstr));

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
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmNumericInstr* instr = (WasmNumericInstr*)malloc(sizeof(WasmNumericInstr));

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
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmNumericInstr* instr = (WasmNumericInstr*)malloc(sizeof(WasmNumericInstr));

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
    FuncType* type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    stack* opds = new_stack();
    stack* ctrls = new_stack();
    ctrl_frame* frame = new_ctrl_frame(opds);
    ctrls->push(ctrls, frame);

    WasmNumericInstr* instr = (WasmNumericInstr*)malloc(sizeof(WasmNumericInstr));

    // Check
    instr->parent.opcode = Op_i32_add;
    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), -2);
    ValueType* value = (ValueType*) malloc(sizeof(ValueType));
    *value = Value_i32;
    opds->push(opds, value);
    EXPECT_EQ(validate_Instr_binop(instr, context, opds, ctrls), -3);

    // Clean
    free(instr);
    clean(opds, ctrls);
    free_Context(context);
    free_WasmModule(module);
}