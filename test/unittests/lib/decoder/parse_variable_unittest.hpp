#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/Variable.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_variable, get_local)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x20\x05";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmVariableInstr* instr = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_get_local);
    EXPECT_EQ(instr->index, 5);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_variable, set_local)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x21\x06";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmVariableInstr* instr = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_set_local);
    EXPECT_EQ(instr->index, 6);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_variable, tee_local)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x22\x0a";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmVariableInstr* instr = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_tee_local);
    EXPECT_EQ(instr->index, 10);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_variable, get_global)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x23\x0c";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmVariableInstr* instr = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_get_global);
    EXPECT_EQ(instr->index, 12);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_variable, set_global)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x24\x0d";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmVariableInstr* instr = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_set_global);
    EXPECT_EQ(instr->index, 13);
    free_WasmFunc(func);
}