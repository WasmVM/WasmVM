#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/FuncType.h>
#include <structures/instrs/Numeric.h>
#include <structures/instrs/Parametric.h>
#include <structures/WasmFunc.h>
#include <structures/WasmModule.h>
#include <Opcodes.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(validate_Func, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 3;
    list_push_back(func->body, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 5;
    list_push_back(func->body, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_add;
    list_push_back(func->body, instr);
    WasmParametricInstr* dropInstr = new_WasmParametricInstr();
    dropInstr->parent.opcode = Op_drop;
    list_push_back(func->body, dropInstr);

    // Check
    EXPECT_EQ(validate_Func(func, module), 0);

    free_WasmModule(module);
}

SKYPAT_F(validate_Func, type_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);

    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 3;
    list_push_back(func->body, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 5;
    list_push_back(func->body, instr);
    instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_add;
    list_push_back(func->body, instr);
    WasmParametricInstr* dropInstr = new_WasmParametricInstr();
    dropInstr->parent.opcode = Op_drop;
    list_push_back(func->body, dropInstr);

    // Check
    EXPECT_EQ(validate_Func(func, module), -1);

    free_WasmModule(module);
}