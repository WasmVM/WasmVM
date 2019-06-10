#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/WasmInstr.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_parametric, valid)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x1A\x1B";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_drop);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_select);
    free_WasmFunc(func);
}

