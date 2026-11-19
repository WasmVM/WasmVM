#ifndef WASMVM_PP_INSTANCES_STACK_DEF
#define WASMVM_PP_INSTANCES_STACK_DEF

#include <Types.hpp>
#include <structures/WasmFunc.hpp>
#include <atomic>
#include <stack>
#include <variant>
#include <functional>
#include <optional>
#include <utility>

#include "ModuleInst.hpp"
#include "FuncInst.hpp"
#include "Store.hpp"

namespace WasmVM {

#ifdef WASMVM_DEBUG
struct Debugger;
#endif

struct Label {

    struct Values : public std::stack<Value> {
        Values() : std::stack<Value>(){}
        /// Takes every value, bottom first, and leaves the label empty.
        std::vector<Value> get();
        void insert(std::vector<Value>);
        /// Read-only walk: begin()/end() bottom to top, rbegin()/rend() top to bottom.
        using const_iterator = std::stack<Value>::container_type::const_iterator;
        using const_reverse_iterator = std::stack<Value>::container_type::const_reverse_iterator;
        const_iterator begin() const { return c.cbegin(); }
        const_iterator end() const { return c.cend(); }
        const_reverse_iterator rbegin() const { return c.crbegin(); }
        const_reverse_iterator rend() const { return c.crend(); }
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
    struct Labels : public std::stack<Label> {
        /// Read-only walk: begin()/end() bottom to top, rbegin()/rend() top to bottom.
        using const_iterator = std::stack<Label>::container_type::const_iterator;
        using const_reverse_iterator = std::stack<Label>::container_type::const_reverse_iterator;
        const_iterator begin() const { return c.cbegin(); }
        const_iterator end() const { return c.cend(); }
        const_reverse_iterator rbegin() const { return c.crbegin(); }
        const_reverse_iterator rend() const { return c.crend(); }
    };
    Labels labels;
    std::vector<Value> locals;
    ModuleInst& module;
    index_t funcaddr;
};

struct Stack {
    /// \param interrupt Optional external interrupt flag, owned by the embedder
    ///   and typically set from another thread. When it reads true, run() throws
    ///   Exception::interrupted at the top of the interpreter loop; the guest
    ///   cannot catch it (try_table/catch_all only handles wasm-level throws).
    ///   A Stack whose run() was interrupted is abandoned — there is no resume.
    ///   Note: the flag is polled between instructions, so a host function
    ///   (hostfunc_t) is never interrupted mid-call; long-running host functions
    ///   may poll stack.interrupt themselves.
#ifdef WASMVM_DEBUG
    /// \param debugger Optional debugger (<Debugger.hpp>), owned by the
    ///   embedder; run() stops through it between wasm instructions.
    Stack(Store& store, const std::atomic<bool>* interrupt = nullptr, Debugger* debugger = nullptr) : store(store), interrupt(interrupt), debugger(debugger){}
#else
    Stack(Store& store, const std::atomic<bool>* interrupt = nullptr) : store(store), interrupt(interrupt){}
#endif
    void invoke(index_t funcaddr, std::vector<Value> args);
    /// Runs until the frame stack is empty and returns the results.
    /// Throws (all declared in the public <exception.hpp>):
    ///   Exception::Trap        the guest trapped (catch the base class);
    ///   Exception::Uncaught    a wasm `throw` no guest try_table caught;
    ///   Exception::interrupted the embedder's interrupt flag was set.
    /// Anything else escaping is a WasmVM invariant violation.
    std::vector<Value> run();
    Store& store;
    const std::atomic<bool>* interrupt;
#ifdef WASMVM_DEBUG
    Debugger* debugger;
#endif
    struct Frames : public std::stack<Frame> {
        /// Read-only walk: begin()/end() bottom to top, rbegin()/rend() top to bottom.
        using const_iterator = std::stack<Frame>::container_type::const_iterator;
        using const_reverse_iterator = std::stack<Frame>::container_type::const_reverse_iterator;
        const_iterator begin() const { return c.cbegin(); }
        const_iterator end() const { return c.cend(); }
        const_reverse_iterator rbegin() const { return c.crbegin(); }
        const_reverse_iterator rend() const { return c.crend(); }
    };
    Frames frames;
    std::vector<Value> results;
};

}

#endif