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

SKYPAT_F(decode_magic_version, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x00\x61\x73\x6d\x01\x00\x00\x00";
    EXPECT_EQ(parse_magic_version(module, &testBin), 0);
    free_WasmModule(module);
}

SKYPAT_F(decode_magic_version, wrong_magic)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x00\x61\x72\x6d\x01\x00\x00\x00";
    EXPECT_EQ(parse_magic_version(module, &testBin), -1);
    free_WasmModule(module);
}

SKYPAT_F(decode_magic_version, invalid_version)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x00\x61\x73\x6d\x05\x00\x00\x00";
    EXPECT_EQ(parse_magic_version(module, &testBin), -2);
    free_WasmModule(module);
}