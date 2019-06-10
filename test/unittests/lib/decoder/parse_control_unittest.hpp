#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/WasmInstr.h>
#include <structures/instrs/Control.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_control, valid_no_immediate)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x00\x01\x0f";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_unreachable);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_nop);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_return);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_control, valid_block)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x02\x40\x0b";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    WasmControlInstr* instr = list_at(WasmControlInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_block);
    EXPECT_EQ(vector_size(instr->resultTypes), 0);
    free_WasmFunc(func);

    func = new_WasmFunc();
    testBin = (uint8_t*) "\x03\x7f\x0b";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    instr = list_at(WasmControlInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_loop);
    EXPECT_EQ(vector_size(instr->resultTypes), 1);
    EXPECT_EQ(*(vector_at(ValueType*, instr->resultTypes, 0)), Value_i32);
    free_WasmFunc(func);

    func = new_WasmFunc();
    testBin = (uint8_t*) "\x04\x7e\x0b";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    instr = list_at(WasmControlInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_if);
    EXPECT_EQ(vector_size(instr->resultTypes), 1);
    EXPECT_EQ(*(vector_at(ValueType*, instr->resultTypes, 0)), Value_i64);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_control, valid_br_br_if_call)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x0c\x01\x0d\x02\x10\x03";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    WasmControlInstr* instr = list_at(WasmControlInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_br);
    EXPECT_EQ(vector_size(instr->indices), 1);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 0)), 1);
    instr = list_at(WasmControlInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_br_if);
    EXPECT_EQ(vector_size(instr->indices), 1);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 0)), 2);
    instr = list_at(WasmControlInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_call);
    EXPECT_EQ(vector_size(instr->indices), 1);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 0)), 3);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_control, valid_br_table)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x0e\x03\x03\x02\x0a\x0e";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    WasmControlInstr* instr = list_at(WasmControlInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_br_table);
    EXPECT_EQ(vector_size(instr->indices), 4);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 0)), 3);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 1)), 2);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 2)), 10);
    EXPECT_EQ(*(vector_at(uint32_t*, instr->indices, 3)), 14);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_control, invalid_block_result_type)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x02\x50\x0b";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), -2);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_control, invalid_call_indirect)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x11\x00\x10";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), -2);
    free_WasmFunc(func);
}