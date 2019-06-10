#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmTable.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_table, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x04\x05\x01\x70\x01\x03\x05";
    EXPECT_EQ(parse_table_section(module, &testBin, testBin + 6), 0);
    EXPECT_EQ(vector_size(module->tables), 1);
    EXPECT_EQ((vector_at(WasmTable*, module->tables, 0))->elemType, TYPE_Table_anyfunc);
    EXPECT_EQ((vector_at(WasmTable*, module->tables, 0))->min, 3);
    EXPECT_EQ((vector_at(WasmTable*, module->tables, 0))->max, 5);
    free_WasmModule(module);
}

SKYPAT_F(decode_table, more_then_one_table)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x04\x09\x02\x70\x01\x03\x05\x70\x01\x03\x05";
    EXPECT_EQ(parse_table_section(module, &testBin, testBin + 10), -1);
    free_WasmModule(module);
}
SKYPAT_F(decode_table, not_anyfunc)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x04\x05\x01\x71\x01\x03\x05";
    EXPECT_EQ(parse_table_section(module, &testBin, testBin + 6), -2);
    free_WasmModule(module);
}