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
    BlockInstr(): Base(OP){}
    BlockInstr(index_t type): Base(OP), type(type){}
    std::optional<index_t> type;
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
using Br_if = OneIndex<Opcode::Br_if>;
struct Br_table : public Base {
    Br_table() : Base(Opcode::Br_table) {}
    std::vector<index_t> indices;
};
using Return = Atomic<Opcode::Return>;
struct Call_indirect : public Base {
    Call_indirect() : Base(Opcode::Call_indirect) {}
    Call_indirect(index_t tableidx, index_t typeidx) : Base(Opcode::Call_indirect), tableidx(tableidx), typeidx(typeidx) {}
    index_t tableidx;
    index_t typeidx;
};
struct Ref_null : public Base {
    Ref_null() : Base(Opcode::Ref_null), heaptype(RefType::funcref) {}
    Ref_null(RefType heaptype) : Base(Opcode::Ref_null), heaptype(heaptype) {}
    RefType heaptype;
};

}

using WasmInstr = std::variant<
    Instr::Unreachable,
    Instr::Nop,
    Instr::Block,
    Instr::Loop,
    Instr::If,
    Instr::Else,
    Instr::End,
    Instr::Br,
    Instr::Br_if,
    Instr::Br_table,
    Instr::Return,
    Instr::Call,
    Instr::Call_indirect,
    Instr::Ref_null
>;

}

#endif