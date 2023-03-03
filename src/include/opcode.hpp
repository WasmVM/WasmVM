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
};

}

}

#endif