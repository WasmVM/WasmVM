#ifndef WASMVM_PP_INSTANCES_STACK_DEF
#define WASMVM_PP_INSTANCES_STACK_DEF

#include <Types.hpp>
#include <structures/WasmFunc.hpp>
#include <stack>
#include <variant>
#include <functional>

#include "ModuleInst.hpp"
#include "FuncInst.hpp"
#include "Store.hpp"

namespace WasmVM {

struct Label {

    struct Values : public std::stack<Value> {
        std::vector<Value> get();
        void insert(std::vector<Value>);
    };

    size_t arity;
    std::vector<WasmInstr>::iterator current;
    std::vector<WasmInstr>::iterator continuation;
    Values values;
};

struct Frame {
    Frame(ModuleInst& module) : module(module){}
    std::stack<Label> labels;
    std::vector<Value> locals;
    ModuleInst& module;
    std::vector<WasmInstr>::iterator end;
};

struct Stack {
    Stack(Store& store) : store(store){}
    void invoke(FuncInst funcinst, std::vector<Value> args);
    std::vector<Value> run();
    Store& store;
    std::stack<Frame> frames;
    std::vector<Value> results;
};

}

#endif