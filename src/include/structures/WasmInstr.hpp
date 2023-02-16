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

template<Opcode::Opcode OP>
struct BlockInstr : public Base {
    BlockInstr(index_t type): Base(OP), type(type){}
    BlockInstr(std::optional<ValueType> type): Base(OP), type(type){}
    std::variant<std::optional<ValueType>, index_t> type;
};

using Unreachable = Atomic<Opcode::Unreachable>;
using Nop = Atomic<Opcode::Nop>;
using End = Atomic<Opcode::End>;
using Call = OneIndex<Opcode::Call>;
using Block = BlockInstr<Opcode::Block>;
using Loop = BlockInstr<Opcode::Loop>;
using If = BlockInstr<Opcode::If>;
using Else = Atomic<Opcode::Else>;
using Br = OneIndex<Opcode::Br>;

}

using WasmInstr = std::variant<
    Instr::Unreachable,
    Instr::Nop,
    Instr::Block,
    Instr::Loop,
    Instr::If,
    Instr::Else,
    Instr::End,
    Instr::Call,
    Instr::Br
>;

}

#endif