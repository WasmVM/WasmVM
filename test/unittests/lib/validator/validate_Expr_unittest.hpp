#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/list.h>
#include <dataTypes/vector.h>
#include <dataTypes/Value.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/WasmInstr.h>
#include <structures/instrs/Numeric.h>
#include <structures/instrs/Parametric.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Opcodes.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(validate_Expr, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    list exprs = new_list((void(*)(void*))free_WasmInstr);

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 3;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 5;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_add;
    list_push_back(exprs, instr);
    WasmParametricInstr* dropInstr = new_WasmParametricInstr();
    dropInstr->parent.opcode = Op_drop;
    list_push_back(exprs, dropInstr);

    // Check
    EXPECT_EQ(validate_Expr(exprs, context), 0);

    free_Context(context);
    free_list(exprs);
    free_WasmModule(module);
}

SKYPAT_F(validate_Expr, no_such_instruction)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    list exprs = new_list((void(*)(void*))free_WasmInstr);

    WasmInstr* instr = (WasmInstr*) malloc(sizeof(WasmInstr));
    instr->opcode = 0xFF;
    instr->free = (void(*)(WasmInstr*))free;
    list_push_back(exprs, instr);

    // Check
    EXPECT_EQ(validate_Expr(exprs, context), -1);

    free_Context(context);
    free_list(exprs);
    free_WasmModule(module);
}

SKYPAT_F(validate_Expr, remain_operand)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    list exprs = new_list((void(*)(void*))free_WasmInstr);

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 3;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 5;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_add;
    list_push_back(exprs, instr);

    // Check
    EXPECT_EQ(validate_Expr(exprs, context), -2);

    free_Context(context);
    free_list(exprs);
    free_WasmModule(module);
}

SKYPAT_F(validate_Expr, wrong_type_of_result)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    FuncType type = new_FuncType();
    ValueType* result = (ValueType*) malloc(sizeof(ValueType));
    *result = Value_i64;
    type->results->push_back(type->results, result);
    module->types->push_back(module->types, type);
    Context* context = new_Context(module, func);
    list exprs = new_list((void(*)(void*))free_WasmInstr);

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 3;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 5;
    list_push_back(exprs, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_add;
    list_push_back(exprs, instr);

    // Check
    EXPECT_EQ(validate_Expr(exprs, context), -3);

    free_Context(context);
    free_list(exprs);
    free_WasmModule(module);
}