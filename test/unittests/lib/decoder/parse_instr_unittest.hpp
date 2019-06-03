#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_instr, valid)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x6a";
    EXPECT_EQ(parseInstr(func, &testBin, testBin), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i32_add);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_instr, unknown_instr)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\xff";
    EXPECT_EQ(parseInstr(func, &testBin, testBin), -1);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_instr, invalid_instr)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x02\x50";
    EXPECT_EQ(parseInstr(func, &testBin, testBin), -2);
    free_WasmFunc(func);
}