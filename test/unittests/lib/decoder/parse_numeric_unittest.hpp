#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Opcodes.h>
#include <structures/WasmFunc.h>
#include <structures/instrs/Numeric.h>
#include "parseInstr.h"
}
#undef _Bool

SKYPAT_F(decoder_parse_numeric, valid_const)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x41\x03\x42\x05\x43\x00\x00\x00\x00\x44\x00\x00\x00\x00\x00\x00\x00\x00";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    WasmNumericInstr* instr = list_at(WasmNumericInstr*, func->body, 0);
    EXPECT_EQ(instr->parent.opcode, Op_i32_const);
    EXPECT_EQ(instr->constant.type, Value_i32);
    EXPECT_EQ(instr->constant.value.i32, 3);
    instr = list_at(WasmNumericInstr*, func->body, 1);
    EXPECT_EQ(instr->parent.opcode, Op_i64_const);
    EXPECT_EQ(instr->constant.type, Value_i64);
    EXPECT_EQ(instr->constant.value.i64, 5);
    instr = list_at(WasmNumericInstr*, func->body, 2);
    EXPECT_EQ(instr->parent.opcode, Op_f32_const);
    EXPECT_EQ(instr->constant.type, Value_f32);
    EXPECT_EQ(instr->constant.value.f32, 0);
    instr = list_at(WasmNumericInstr*, func->body, 3);
    EXPECT_EQ(instr->parent.opcode, Op_f64_const);
    EXPECT_EQ(instr->constant.type, Value_f64);
    EXPECT_EQ(instr->constant.value.f64, 0);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_i32_relop)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i32_eqz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_i32_eq);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_i32_ne);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_i32_lt_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_i32_lt_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_i32_gt_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_i32_gt_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_i32_le_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_i32_le_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_i32_ge_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_i32_ge_u);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_i64_relop)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 10), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i64_eqz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_i64_eq);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_i64_ne);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_i64_lt_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_i64_lt_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_i64_gt_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_i64_gt_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_i64_le_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_i64_le_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_i64_ge_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_i64_ge_u);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_f32_relop)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x5b\x5c\x5d\x5e\x5f\x60";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_f32_eq);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_f32_ne);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_f32_lt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_f32_gt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_f32_le);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_f32_ge);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_f64_relop)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x61\x62\x63\x64\x65\x66";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 5), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_f64_eq);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_f64_ne);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_f64_lt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_f64_gt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_f64_le);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_f64_ge);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_i32_op)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i32_clz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_i32_ctz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_i32_popcnt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_i32_add);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_i32_sub);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_i32_mul);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_i32_div_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_i32_div_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_i32_rem_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_i32_rem_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_i32_and);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 11))->opcode, Op_i32_or);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 12))->opcode, Op_i32_xor);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 13))->opcode, Op_i32_shl);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 14))->opcode, Op_i32_shr_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 15))->opcode, Op_i32_shr_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 16))->opcode, Op_i32_rotl);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 17))->opcode, Op_i32_rotr);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_i64_op)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 17), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i64_clz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_i64_ctz);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_i64_popcnt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_i64_add);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_i64_sub);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_i64_mul);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_i64_div_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_i64_div_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_i64_rem_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_i64_rem_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_i64_and);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 11))->opcode, Op_i64_or);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 12))->opcode, Op_i64_xor);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 13))->opcode, Op_i64_shl);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 14))->opcode, Op_i64_shr_s);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 15))->opcode, Op_i64_shr_u);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 16))->opcode, Op_i64_rotl);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 17))->opcode, Op_i64_rotr);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_f32_op)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_f32_abs);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_f32_neg);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_f32_ceil);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_f32_floor);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_f32_trunc);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_f32_nearest);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_f32_sqrt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_f32_add);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_f32_sub);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_f32_mul);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_f32_div);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 11))->opcode, Op_f32_min);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 12))->opcode, Op_f32_max);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 13))->opcode, Op_f32_copysign);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_f64_op)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 13), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_f64_abs);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_f64_neg);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_f64_ceil);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_f64_floor);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_f64_trunc);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_f64_nearest);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_f64_sqrt);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_f64_add);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_f64_sub);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_f64_mul);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_f64_div);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 11))->opcode, Op_f64_min);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 12))->opcode, Op_f64_max);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 13))->opcode, Op_f64_copysign);
    free_WasmFunc(func);
}

SKYPAT_F(decoder_parse_numeric, valid_convert_op)
{
    WasmFunc* func = new_WasmFunc();
    uint8_t* testBin = (uint8_t*) "\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf";
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ(parseInstr(func, &testBin, testBin + 24), 0);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 0))->opcode, Op_i32_wrap_i64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 1))->opcode, Op_i32_trunc_s_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 2))->opcode, Op_i32_trunc_u_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 3))->opcode, Op_i32_trunc_s_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 4))->opcode, Op_i32_trunc_u_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 5))->opcode, Op_i64_extend_s_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 6))->opcode, Op_i64_extend_u_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 7))->opcode, Op_i64_trunc_s_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 8))->opcode, Op_i64_trunc_u_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 9))->opcode, Op_i64_trunc_s_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 10))->opcode, Op_i64_trunc_u_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 11))->opcode, Op_f32_convert_s_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 12))->opcode, Op_f32_convert_u_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 13))->opcode, Op_f32_convert_s_i64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 14))->opcode, Op_f32_convert_u_i64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 15))->opcode, Op_f32_demote_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 16))->opcode, Op_f64_convert_s_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 17))->opcode, Op_f64_convert_u_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 18))->opcode, Op_f64_convert_s_i64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 19))->opcode, Op_f64_convert_u_i64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 20))->opcode, Op_f64_promote_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 21))->opcode, Op_i32_reinterpret_f32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 22))->opcode, Op_i64_reinterpret_f64);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 23))->opcode, Op_f32_reinterpret_i32);
    EXPECT_EQ((list_at(WasmInstr*, func->body, 24))->opcode, Op_f64_reinterpret_i64);
    free_WasmFunc(func);
}