#ifndef WASMVM_PP_STRUCTURE_INSTR
#define WASMVM_PP_STRUCTURE_INSTR

#include <variant>
#include <opcode.hpp>

namespace WasmVM {

namespace Instr {

struct Base {
    Base(Opcode::opcode_t op) : opcode(op) {}
    Opcode::opcode_t opcode;
};

template<Opcode::Opcode OP>
struct Atomic : public Base {
    Atomic() : Base(OP) {}
};

template<Opcode::Opcode OP>
struct OneIndex : public Base {
    OneIndex() : Base(OP) {}
    OneIndex(index_t index) : Base(OP), index(index){}
    index_t index;
};

using Unreachable = Atomic<Opcode::Unreachable>;
using Nop = Atomic<Opcode::Nop>;
using Call = OneIndex<Opcode::Call>;

}

using WasmInstr = std::variant<
    Instr::Unreachable,
    Instr::Nop,
    Instr::Call
>;

}

#endif