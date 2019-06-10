#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmMemory.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_memory, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x05\x04\x01\x01\x03\x05";
    EXPECT_EQ(parse_memory_section(module, &testBin, testBin + 5), 0);
    EXPECT_EQ(vector_size(module->mems), 1);
    EXPECT_EQ((vector_at(WasmMemory*, module->mems, 0))->min, 3);
    EXPECT_EQ((vector_at(WasmMemory*, module->mems, 0))->max, 5);
    free_WasmModule(module);
}

SKYPAT_F(decode_memory, more_then_one_memory)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x05\x07\x02\x01\x03\x05\x01\x03\x05";
    EXPECT_EQ(parse_memory_section(module, &testBin, testBin + 8), -1);
    free_WasmModule(module);
}