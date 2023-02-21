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
    Ref_is_null = 0xD1,
    Ref_func = 0xD2,
    // Parametric
    #define Op_drop                 0x1A
    #define Op_select               0x1B
};

}

}

#endif