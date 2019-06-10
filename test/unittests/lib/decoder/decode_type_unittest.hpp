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

SKYPAT_F(decode_type, valid)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x01\x12\x04\x60\x00\x01\x7f\x60\x02\x7f\x7d\x01\x7f\x60\x01\x7d\x00\x60\x00\x00";
    EXPECT_EQ(parse_type_section(module, &testBin, testBin + 19), 0);
    EXPECT_EQ(vector_size(module->types), 4);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 0))->params), 0);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 0))->results), 1);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 1))->params), 2);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 1))->results), 1);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 2))->params), 1);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 2))->results), 0);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 3))->params), 0);
    EXPECT_EQ(vector_size((vector_at(FuncType, module->types, 3))->results), 0);
    free_WasmModule(module);
}

SKYPAT_F(decode_type, FuncType_wrong_code)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x01\x12\x04\x61\x00\x01\x7f\x60\x02\x7f\x7d\x01\x7f\x60\x01\x7d\x00\x60\x00\x00";
    EXPECT_EQ(parse_type_section(module, &testBin, testBin + 19), -1);
    free_WasmModule(module);
}

SKYPAT_F(decode_type, no_such_param_type)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x01\x12\x04\x60\x00\x01\x7f\x60\x02\x70\x7d\x01\x7f\x60\x01\x7d\x00\x60\x00\x00";
    EXPECT_EQ(parse_type_section(module, &testBin, testBin + 19), -2);
    free_WasmModule(module);
}

SKYPAT_F(decode_type, no_such_result_type)
{
    WasmModule* module = new_WasmModule((char*)"Test");
    uint8_t* testBin = (uint8_t*) "\x01\x12\x04\x60\x00\x01\x70\x60\x02\x70\x7d\x01\x7f\x60\x01\x7d\x00\x60\x00\x00";
    EXPECT_EQ(parse_type_section(module, &testBin, testBin + 19), -3);
    free_WasmModule(module);
}