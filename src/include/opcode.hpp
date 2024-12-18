#ifndef WASMVM_PP_OPCODE
#define WASMVM_PP_OPCODE

#include <cstdint>
#include <Types.hpp>

namespace WasmVM {

namespace Opcode {

using opcode_t = uint16_t;

enum Opcode : opcode_t {
    // Control
    Unreachable = 0x00,
    Nop = 0x01,
    Block = 0x02,
    Loop = 0x03,
    If = 0x04,
    Else = 0x05,
    End = 0x0b,
    Br = 0x0c,
    Br_if = 0x0d,
    Br_table = 0x0e,
    Return = 0x0f,
    Call = 0x10,
    Call_indirect = 0x11,
    // Reference
    Ref_null = 0xd0,
    Ref_is_null = 0xd1,
    Ref_func = 0xd2,
    // Parametric
    Drop = 0x1a,
    Select = 0x1b,
    Select_t = 0x1c,
    // Variable
    Local_get = 0x20,
    Local_set = 0x21,
    Local_tee = 0x22,
    Global_get = 0x23,
    Global_set = 0x24,
    // Table
    Table_get = 0x25,
    Table_set = 0x26,
    Table_init = 0xFC0C,
    Elem_drop = 0xFC0D,
    Table_copy = 0xFC0E,
    Table_grow = 0xFC0F,
    Table_size = 0xFC10,
    Table_fill = 0xFC11,
    // Memory
    I32_load = 0x28,
    I64_load = 0x29,
    F32_load = 0x2A,
    F64_load = 0x2B,
    I32_load8_s = 0x2C,
    I32_load8_u = 0x2D,
    I32_load16_s = 0x2E,
    I32_load16_u = 0x2F,
    I64_load8_s = 0x30,
    I64_load8_u = 0x31,
    I64_load16_s = 0x32,
    I64_load16_u = 0x33,
    I64_load32_s = 0x34,
    I64_load32_u = 0x35,
    I32_store = 0x36,
    I64_store = 0x37,
    F32_store = 0x38,
    F64_store = 0x39,
    I32_store8 = 0x3A,
    I32_store16 = 0x3B,
    I64_store8 = 0x3C,
    I64_store16 = 0x3D,
    I64_store32 = 0x3E,
    Memory_size = 0x3F,
    Memory_grow = 0x40,
    Memory_init = 0xFC08,
    Data_drop = 0xFC09,
    Memory_copy = 0xFC0A,
    Memory_fill = 0xFC0B,
    // Numeric
    I32_const = 0x41,
    I64_const = 0x42,
    F32_const = 0x43,
    F64_const = 0x44,
    I32_eqz = 0x45,
    I32_eq = 0x46,
    I32_ne = 0x47,
    I32_lt_s = 0x48,
    I32_lt_u = 0x49,
    I32_gt_s = 0x4A,
    I32_gt_u = 0x4B,
    I32_le_s = 0x4C,
    I32_le_u = 0x4D,
    I32_ge_s = 0x4E,
    I32_ge_u = 0x4F,
    I64_eqz = 0x50,
    I64_eq = 0x51,
    I64_ne = 0x52,
    I64_lt_s = 0x53,
    I64_lt_u = 0x54,
    I64_gt_s = 0x55,
    I64_gt_u = 0x56,
    I64_le_s = 0x57,
    I64_le_u = 0x58,
    I64_ge_s = 0x59,
    I64_ge_u = 0x5A,
    F32_eq = 0x5B,
    F32_ne = 0x5C,
    F32_lt = 0x5D,
    F32_gt = 0x5E,
    F32_le = 0x5F,
    F32_ge = 0x60,
    F64_eq = 0x61,
    F64_ne = 0x62,
    F64_lt = 0x63,
    F64_gt = 0x64,
    F64_le = 0x65,
    F64_ge = 0x66,
    I32_clz = 0x67,
    I32_ctz = 0x68,
    I32_popcnt = 0x69,
    I32_add = 0x6A,
    I32_sub = 0x6B,
    I32_mul = 0x6C,
    I32_div_s = 0x6D,
    I32_div_u = 0x6E,
    I32_rem_s = 0x6F,
    I32_rem_u = 0x70,
    I32_and = 0x71,
    I32_or = 0x72,
    I32_xor = 0x73,
    I32_shl = 0x74,
    I32_shr_s = 0x75,
    I32_shr_u = 0x76,
    I32_rotl = 0x77,
    I32_rotr = 0x78,
    I64_clz = 0x79,
    I64_ctz = 0x7A,
    I64_popcnt = 0x7B,
    I64_add = 0x7C,
    I64_sub = 0x7D,
    I64_mul = 0x7E,
    I64_div_s = 0x7F,
    I64_div_u = 0x80,
    I64_rem_s = 0x81,
    I64_rem_u = 0x82,
    I64_and = 0x83,
    I64_or = 0x84,
    I64_xor = 0x85,
    I64_shl = 0x86,
    I64_shr_s = 0x87,
    I64_shr_u = 0x88,
    I64_rotl = 0x89,
    I64_rotr = 0x8A,
    F32_abs = 0x8B,
    F32_neg = 0x8C,
    F32_ceil = 0x8D,
    F32_floor = 0x8E,
    F32_trunc = 0x8F,
    F32_nearest = 0x90,
    F32_sqrt = 0x91,
    F32_add = 0x92,
    F32_sub = 0x93,
    F32_mul = 0x94,
    F32_div = 0x95,
    F32_min = 0x96,
    F32_max = 0x97,
    F32_copysign = 0x98,
    F64_abs = 0x99,
    F64_neg = 0x9A,
    F64_ceil = 0x9B,
    F64_floor = 0x9C,
    F64_trunc = 0x9D,
    F64_nearest = 0x9E,
    F64_sqrt = 0x9F,
    F64_add = 0xA0,
    F64_sub = 0xA1,
    F64_mul = 0xA2,
    F64_div = 0xA3,
    F64_min = 0xA4,
    F64_max = 0xA5,
    F64_copysign = 0xA6,
    I32_wrap_i64 = 0xA7,
    I32_trunc_f32_s = 0xA8,
    I32_trunc_f32_u = 0xA9,
    I32_trunc_f64_s = 0xAA,
    I32_trunc_f64_u = 0xAB,
    I64_extend_i32_s = 0xAC,
    I64_extend_i32_u = 0xAD,
    I64_trunc_f32_s = 0xAE,
    I64_trunc_f32_u = 0xAF,
    I64_trunc_f64_s = 0xB0,
    I64_trunc_f64_u = 0xB1,
    F32_convert_i32_s = 0xB2,
    F32_convert_i32_u = 0xB3,
    F32_convert_i64_s = 0xB4,
    F32_convert_i64_u = 0xB5,
    F32_demote_f64 = 0xB6,
    F64_convert_i32_s = 0xB7,
    F64_convert_i32_u = 0xB8,
    F64_convert_i64_s = 0xB9,
    F64_convert_i64_u = 0xBA,
    F64_promote_f32 = 0xBB,
    I32_reinterpret_f32 = 0xBC,
    I64_reinterpret_f64 = 0xBD,
    F32_reinterpret_i32 = 0xBE,
    F64_reinterpret_i64 = 0xBF,
    I32_extend8_s = 0xC0,
    I32_extend16_s = 0xC1,
    I64_extend8_s = 0xC2,
    I64_extend16_s = 0xC3,
    I64_extend32_s = 0xC4,
    I32_trunc_sat_f32_s = 0xFC00,
    I32_trunc_sat_f32_u = 0xFC01,
    I32_trunc_sat_f64_s = 0xFC02,
    I32_trunc_sat_f64_u = 0xFC03,
    I64_trunc_sat_f32_s = 0xFC04,
    I64_trunc_sat_f32_u = 0xFC05,
    I64_trunc_sat_f64_s = 0xFC06,
    I64_trunc_sat_f64_u = 0xFC07,
};

}

}

#endif