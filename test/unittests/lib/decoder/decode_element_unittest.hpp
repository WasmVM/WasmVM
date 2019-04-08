#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <structures/WasmModule.h>
#include <structures/WasmElem.h>
#include "sections.h"
}
#undef _Bool

SKYPAT_F(decode_elem, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x09\x09\x01\x00\x41\x0a\x0b\x03\x00\x03\x05";
    EXPECT_EQ(parse_element_section(module, &testBin, testBin + 10), 0);
    EXPECT_EQ(module->elems->length, 1);
    EXPECT_EQ(((WasmElem*)module->elems->at(module->elems, 0))->table, 0);
    EXPECT_EQ(((WasmElem*)module->elems->at(module->elems, 0))->offset.type, Value_i32);
    EXPECT_EQ(((WasmElem*)module->elems->at(module->elems, 0))->offset.value.i32, 10);
    EXPECT_EQ(((WasmElem*)module->elems->at(module->elems, 0))->init->length, 3);
    free_WasmModule(module);
}

SKYPAT_F(decode_elem, invalid_table_index)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x09\x09\x01\x03\x41\x0a\x0b\x03\x00\x03\x05";
    EXPECT_EQ(parse_element_section(module, &testBin, testBin + 10), -1);
    free_WasmModule(module);
}

SKYPAT_F(decode_elem, invalid_offset_instrction)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x09\x09\x01\x00\x60\x0a\x0b\x03\x00\x03\x05";
    EXPECT_EQ(parse_element_section(module, &testBin, testBin + 10), -2);
    free_WasmModule(module);
}