#ifndef WASMVM_PP_INVOKE_RunVisitor_DEF
#define WASMVM_PP_INVOKE_RunVisitor_DEF

#include <instances/Stack.hpp>
#include <optional>
#include <vector>
#include <exception.hpp>

using namespace WasmVM;

struct RunVisitor {
    RunVisitor(Stack& stack) : stack(stack){};

    template<typename T> requires (std::is_base_of<WasmVM::Instr::Base, T>::value)
    void operator()(T& instr){
        throw Exception::Exception("unimplemented");
    }

    void operator()(Instr::Unreachable&);
    void operator()(Instr::Nop&);
    void operator()(Instr::Block&);
    void operator()(Instr::Loop&);
    void operator()(Instr::If&);
    void operator()(Instr::Else&);
    void operator()(Instr::End&);
    void operator()(Instr::Br&);
    void operator()(Instr::Br_if&);
    void operator()(Instr::Br_table&);
    void operator()(Instr::Return&);
    void operator()(Instr::Call&);
    void operator()(Instr::Call_indirect&);

    void operator()(Instr::I32_const&);

    Stack& stack;
};

#endif