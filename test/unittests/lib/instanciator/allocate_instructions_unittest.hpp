#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <Allocates.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/list_p.h>
#include <dataTypes/Value.h>
#include <Opcodes.h>
#include <structures/instrs/Control.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Parametric.h>
#include <structures/instrs/Memory.h>
#include <structures/instrs/Numeric.h>
#include <instance/ControlInstrInst.h>
#include <instance/ParametricInstrInst.h>
#include <instance/MemoryInstrInst.h>
#include <instance/VariableInstrInst.h>
#include <instance/NumericInstrInst.h>
}
#undef _Bool

SKYPAT_F(allocate_instructions, control)
{
    // Prepare
    WasmControlInstr* instr = new_WasmControlInstr();
    instr->parent.opcode = Op_br_table;
    ValueType* result1 = (ValueType*) malloc(sizeof(ValueType));
    *result1 = Value_i32;
    vector_push_back(instr->resultTypes, result1);
    ValueType* result2 = (ValueType*) malloc(sizeof(ValueType));
    *result2 = Value_f32;
    vector_push_back(instr->resultTypes, result2);
    uint32_t* index1 = (uint32_t*) malloc(sizeof(uint32_t));
    *index1 = 2;
    vector_push_back(instr->indices, index1);
    uint32_t* index2 = (uint32_t*) malloc(sizeof(uint32_t));
    *index2 = 4;
    vector_push_back(instr->indices, index2);
    list_p body = new_list_p((void(*)(void*))free_WasmControlInstr);
    list_push_back(body, instr);

    // Test
    ControlInstrInst* instrInst = (ControlInstrInst*)allocate_Instruction(body, 0);
    EXPECT_EQ(instrInst->parent.opcode, Op_br_table);
    EXPECT_EQ(vector_size(instrInst->resultTypes), 2);
    EXPECT_EQ(*vector_at(ValueType*, instrInst->resultTypes, 0), Value_i32);
    EXPECT_EQ(*vector_at(ValueType*, instrInst->resultTypes, 1), Value_f32);
    EXPECT_EQ(vector_size(instrInst->indices), 2);
    EXPECT_EQ(*vector_at(uint32_t*, instrInst->indices, 0), 2);
    EXPECT_EQ(*vector_at(uint32_t*, instrInst->indices, 1), 4);

    // Clean
    free_ControlInstrInst(instrInst);
    free_WasmControlInstr(instr);
}

SKYPAT_F(allocate_instructions, parametric)
{
    // Prepare
    WasmParametricInstr* instr = new_WasmParametricInstr();
    instr->parent.opcode = Op_drop;
    list_p body = new_list_p((void(*)(void*))free_WasmParametricInstr);
    list_push_back(body, instr);

    // Test
    ParametricInstrInst* instrInst = (ParametricInstrInst*)allocate_Instruction(body, 0);
    EXPECT_EQ(instrInst->opcode, Op_drop);

    // Clean
    free_ParametricInstrInst(instrInst);
    free_WasmParametricInstr(instr);
}

SKYPAT_F(allocate_instructions, memory)
{
    // Prepare
    WasmMemoryInstr* instr = new_WasmMemoryInstr(16, 87);
    instr->parent.opcode = Op_i32_store;
    list_p body = new_list_p((void(*)(void*))free_WasmMemoryInstr);
    list_push_back(body, instr);

    // Test
    MemoryInstrInst* instrInst = (MemoryInstrInst*)allocate_Instruction(body, 0);
    EXPECT_EQ(instrInst->parent.opcode, Op_i32_store);
    EXPECT_EQ(instrInst->align, 16);
    EXPECT_EQ(instrInst->offset, 87);

    // Clean
    free_MemoryInstrInst(instrInst);
    free_WasmMemoryInstr(instr);
}

SKYPAT_F(allocate_instructions, numeric)
{
    // Prepare
    WasmNumericInstr* instr = new_WasmNumericInstr();
    instr->parent.opcode = Op_i32_const;
    instr->constant.parent.entryType = Entry_Value;
    instr->constant.type = Value_i32;
    instr->constant.value.i32 = 45;
    list_p body = new_list_p((void(*)(void*))free_WasmNumericInstr);
    list_push_back(body, instr);

    // Test
    NumericInstrInst* instrInst = (NumericInstrInst*)allocate_Instruction(body, 0);
    EXPECT_EQ(instrInst->parent.opcode, Op_i32_const);
    EXPECT_EQ(instr->constant.parent.entryType, Entry_Value);
    EXPECT_EQ(instr->constant.type, Value_i32);
    EXPECT_EQ(instr->constant.value.i32, 45);

    // Clean
    free_NumericInstrInst(instrInst);
    free_WasmNumericInstr(instr);
}