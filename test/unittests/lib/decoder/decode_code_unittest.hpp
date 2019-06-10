#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Numeric.h>
#include <Opcodes.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_code, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    vector_push_back(module->funcs, new_WasmFunc());
    uint8_t* testBin = (uint8_t*) "\x0a\x0d\x01\x0b\x02\x01\x7f\x01\x7d\x20\x00\x20\x01\x6a\x0b";
    EXPECT_EQ(parse_code_section(module, &testBin, testBin + 14), 0);
    WasmFunc* func = vector_at(WasmFunc*, module->funcs, 0);
    EXPECT_EQ(vector_size(func->locals), 2);
    EXPECT_EQ(*(vector_at(ValueType*, func->locals, 0)), Value_i32);
    EXPECT_EQ(*(vector_at(ValueType*, func->locals, 1)), Value_f32);
    EXPECT_EQ(list_size(func->body), 3);
    WasmVariableInstr* get_local = list_at(WasmVariableInstr*, func->body, 0);
    EXPECT_EQ(get_local->parent.opcode, Op_get_local);
    EXPECT_EQ(get_local->index, 0);
    get_local = list_at(WasmVariableInstr*, func->body, 1);
    EXPECT_EQ(get_local->parent.opcode, Op_get_local);
    EXPECT_EQ(get_local->index, 1);
    WasmNumericInstr* i32_add = list_at(WasmNumericInstr*, func->body, 2);
    EXPECT_EQ(i32_add->parent.opcode, Op_i32_add);
    free_WasmModule(module);
}