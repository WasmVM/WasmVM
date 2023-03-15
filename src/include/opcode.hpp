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
};

}

}

#endif