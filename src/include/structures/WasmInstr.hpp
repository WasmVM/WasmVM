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
struct Core : public Base {
    Core() : Base(OP) {}
};

using Unreachable = Core<Opcode::Unreachable>;

}

using WasmInstr = std::variant<
    Instr::Unreachable
>;

}

#endif