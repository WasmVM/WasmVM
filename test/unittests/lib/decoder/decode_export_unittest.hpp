#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmExport.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_export, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x07\x0b\x01\x07TestExp\x00\x08";
    EXPECT_EQ(parse_export_section(module, &testBin, testBin + 12), 0);
    EXPECT_EQ(vector_size(module->exports), 1);
    EXPECT_FALSE(strcmp((vector_at(WasmExport*, module->exports, 0))->name, "TestExp"));
    EXPECT_EQ((vector_at(WasmExport*, module->exports, 0))->descType, Desc_Func);
    EXPECT_EQ((vector_at(WasmExport*, module->exports, 0))->descIdx, 8);
    free_WasmModule(module);
}

SKYPAT_F(decode_export, unknown_export_type)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x07\x0b\x01\x07TestExp\x10\x08";
    EXPECT_EQ(parse_export_section(module, &testBin, testBin + 12), -1);
    free_WasmModule(module);
}
