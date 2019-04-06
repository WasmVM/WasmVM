#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmData.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_data, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x0b\x08\x01\x00\x41\x00\x0b\x02hi";
    EXPECT_EQ(parse_data_section(module, &testBin, testBin + 9), 0);
    EXPECT_EQ(module->datas->length, 1);
    WasmData* data = (WasmData*)module->datas->at(module->datas, 0);
    EXPECT_EQ(data->data, 0);
    EXPECT_EQ(data->offset.type, Value_i32);
    EXPECT_EQ(data->offset.value.i32, 0);
    EXPECT_FALSE(strncmp((char*)data->init->data, "hi", 2));
    free_WasmModule(module);
}

SKYPAT_F(decode_elem, invalid_table_index)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x0b\x08\x01\x01\x41\x00\x0b\x02hi";
    EXPECT_EQ(parse_data_section(module, &testBin, testBin + 9), -1);
    free_WasmModule(module);
}

SKYPAT_F(decode_elem, invalid_offset_instrction)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x0b\x08\x01\x00\x43\x00\x0b\x02hi";
    EXPECT_EQ(parse_data_section(module, &testBin, testBin + 9), -2);
    free_WasmModule(module);
}