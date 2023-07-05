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

    Stack& stack;
    std::vector<Value> results;
};

#endif