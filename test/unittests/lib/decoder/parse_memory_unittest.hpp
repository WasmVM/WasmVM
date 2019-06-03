#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/Memory.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_memory, memory_size)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x3f\x00";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_memory_size);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, memory_grow)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x40\x00";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 1), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_memory_grow);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, i32_load_x)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x28\x01\x04\x2c\x01\x04\x2d\x01\x04\x2e\x01\x04\x2f\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 14), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 14), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 14), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 14), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 14), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_i32_load);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_i32_load8_s);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_i32_load8_u);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 3);
    EXPECT_EQ(instr->parent.opcode, Op_i32_load16_s);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 4);
    EXPECT_EQ(instr->parent.opcode, Op_i32_load16_u);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, i64_load_x)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x29\x01\x04\x30\x01\x04\x31\x01\x04\x32\x01\x04\x33\x01\x04\x34\x01\x04\x35\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 21), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load8_s);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load8_u);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 3);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load16_s);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 4);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load16_u);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 5);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load32_s);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 6);
    EXPECT_EQ(instr->parent.opcode, Op_i64_load32_u);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, f32_load)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x2a\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_f32_load);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, f64_load)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x2b\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 2), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_f64_load);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, i32_store_x)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x36\x01\x04\x3a\x01\x04\x3b\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 8), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 8), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 8), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_i32_store);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_i32_store8);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_i32_store16);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, i64_store_x)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x37\x01\x04\x3c\x01\x04\x3d\x01\x04\x3e\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 11), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 11), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 11), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 11), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_i64_store);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_i64_store8);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_i64_store16);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    instr = list_at(WasmMemoryInstr*, func->body, 3);
    EXPECT_EQ(instr->parent.opcode, Op_i64_store32);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, f32_store)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x38\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 4), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_f32_store);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_memory, f64_store)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x39\x01\x04";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 4), 0);
    WasmMemoryInstr* instr = list_at(WasmMemoryInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_f64_store);
    EXPECT_EQ(instr->align, 1);
    EXPECT_EQ(instr->offset, 4);
    free_WasmFunc(func);
}