#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_start, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x08\x01\x07";
    EXPECT_EQ(parse_start_section(module, &testBin, testBin + 2), 0);
    EXPECT_EQ(module->start, 7);
    free_WasmModule(module);
}
