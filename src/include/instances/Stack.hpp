#ifndef WASMVM_PP_INSTANCES_STACK_DEF
#define WASMVM_PP_INSTANCES_STACK_DEF

#include <Types.hpp>
#include <structures/WasmFunc.hpp>
#include <stack>
#include <variant>
#include <functional>
#include <optional>
#include <utility>

#include "ModuleInst.hpp"
#include "FuncInst.hpp"
#include "Store.hpp"

namespace WasmVM {

struct Label {

    struct Values : public std::stack<Value> {
        Values() : std::stack<Value>(){}
        std::vector<Value> get();
        void insert(std::vector<Value>);
    };

    struct Counters : public std::pair<size_t, size_t> {
        Counters() : std::pair<size_t, size_t>(), current(first), continuation(second) {}
        size_t& current;
        size_t& continuation;
    };

    struct CatchEntry {
        enum Kind { Catch = 0, CatchRef = 1, CatchAll = 2, CatchAllRef = 3 } kind;
        index_t tag_addr;   // resolved tag address (only valid for Catch/CatchRef)
        index_t label_idx;  // branch target label index
    };

    size_t arity;
    std::optional<Counters> pc;
    Values values;
    std::vector<CatchEntry> catches; // non-empty for try_table labels
};

struct Frame {
    Frame(ModuleInst& module, index_t funcaddr) : module(module), funcaddr(funcaddr){}
    std::stack<Label> labels;
    std::vector<Value> locals;
    ModuleInst& module;
    index_t funcaddr;
};

struct Stack {
    Stack(Store& store) : store(store){}
    void invoke(index_t funcaddr, std::vector<Value> args);
    std::vector<Value> run();
    Store& store;
    std::stack<Frame> frames;
    std::vector<Value> results;
};

}

#endif