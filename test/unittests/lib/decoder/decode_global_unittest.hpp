#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmGlobal.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_global, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x06\x07\x01\x7f\x00\x41\xfb\x00\x0b";
    EXPECT_EQ(parse_global_section(module, &testBin, testBin + 8), 0);
    EXPECT_EQ(vector_size(module->globals), 1);
    EXPECT_FALSE((vector_at(WasmGlobal*, module->globals, 0))->mut);
    EXPECT_EQ((vector_at(WasmGlobal*, module->globals, 0))->valType, Value_i32);
    EXPECT_EQ((vector_at(WasmGlobal*, module->globals, 0))->init.type, Value_i32);
    EXPECT_EQ((vector_at(WasmGlobal*, module->globals, 0))->init.value.i32, 123);
    free_WasmModule(module);
}

SKYPAT_F(decode_global, unknown_global_type)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x06\x07\x01\x60\x00\x41\xfb\x00\x0b";
    EXPECT_EQ(parse_global_section(module, &testBin, testBin + 8), -1);
    free_WasmModule(module);
}

SKYPAT_F(decode_global, invalid_init_expression)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x06\x07\x01\x7f\x00\x47\xfb\x00\x0b";
    EXPECT_EQ(parse_global_section(module, &testBin, testBin + 8), -2);
    free_WasmModule(module);
}