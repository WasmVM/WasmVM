#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <Allocates.h>
#include <structures/instrs/Control.h>
#include <dataTypes/vector.h>
#include <dataTypes/Value.h>
#include <Opcodes.h>
#include <instance/ControlInstrInst.h>
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
    instr->resultTypes->push_back(instr->resultTypes, result1);
    ValueType* result2 = (ValueType*) malloc(sizeof(ValueType));
    *result2 = Value_f32;
    instr->resultTypes->push_back(instr->resultTypes, result2);
    uint32_t* index1 = (uint32_t*) malloc(sizeof(uint32_t));
    *index1 = 2;
    instr->indices->push_back(instr->indices, index1);
    uint32_t* index2 = (uint32_t*) malloc(sizeof(uint32_t));
    *index2 = 4;
    instr->indices->push_back(instr->indices, index2);

    // Test
    ControlInstrInst* instrInst = (ControlInstrInst*)allocate_Instruction((WasmInstr*) instr);
    EXPECT_EQ(instrInst->parent.opcode, Op_br_table);
    EXPECT_EQ(instrInst->resultTypes->length, 2);
    EXPECT_EQ(*(ValueType*)instrInst->resultTypes->at(instrInst->resultTypes, 0), Value_i32);
    EXPECT_EQ(*(ValueType*)instrInst->resultTypes->at(instrInst->resultTypes, 1), Value_f32);
    EXPECT_EQ(instrInst->indices->length, 2);
    EXPECT_EQ(*(uint32_t*)instrInst->indices->at(instrInst->indices, 0), 2);
    EXPECT_EQ(*(uint32_t*)instrInst->indices->at(instrInst->indices, 1), 4);

    // Clean
    free_ControlInstrInst(instrInst);
    free_WasmControlInstr(instr);
}