#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmFunc.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_function, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x03\x03\x02\x0a\x03";
    EXPECT_EQ(parse_func_section(module, &testBin, testBin + 4), 0);
    EXPECT_EQ(vector_size(module->funcs), 2);
    EXPECT_EQ((vector_at(WasmFunc*, module->funcs, 0))->type, 10);
    EXPECT_EQ((vector_at(WasmFunc*, module->funcs, 1))->type, 3);
    free_WasmModule(module);
}
