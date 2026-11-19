#ifndef WASMVM_PP_PROFILER_DEF
#define WASMVM_PP_PROFILER_DEF

// Profiler (#61): per-function instruction counts, call counts and time,
// recorded while Stack::run() runs. Compiled only under WASMVM_PROFILE;
// without it this header declares nothing.

#ifdef WASMVM_PROFILE

#include <Types.hpp>
#include <opcode.hpp>
#include <json.hpp>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

// The interpreter's instruction visitor (src/lib/invoke/RunVisitor.hpp), which
// reports function exits on return and tail calls.
struct RunVisitor;

namespace WasmVM {

struct Stack;
struct Store;
struct ModuleInst;
struct Debugger;

/// Attach to a Stack (or module_instanciate) through Observers to record what
/// the guest runs. It only records: it never stops the VM.
///
/// Every Stack::run() is one section of the report. A run() started while
/// another one on this Profiler is still running (on the same thread) gets a
/// section of its own and ends before the outer one continues; its time still
/// counts for the outer section's open frames.
///
/// Time is read only on function entry and exit. Time a Debugger attached to
/// the same Stack spends stopped is not counted. A function's inclusive time
/// counts only its outermost active call, so recursion is not counted twice;
/// every call adds to its call count and its exclusive time.
///
/// Not thread-safe: one thread drives it.
struct Profiler final {
    /// A clock in nanoseconds. nullptr means std::chrono::steady_clock.
    using clock_t = int64_t (*)();

    explicit Profiler(clock_t now = nullptr);

    /// The name the report gives a module's functions. Without one, a module's
    /// name section module name is used, and with neither the module is "".
    void set_module_name(const ModuleInst& module, std::string name);

    /// Ends every section still running, at this moment: their open frames
    /// exit now. For a program that ends without returning from run(), e.g.
    /// std::exit called inside a host function.
    void finish();

    /// The full report (schema version 1).
    Json::Value report() const;
    /// Collapsed stacks for flame graphs: one line per call path, frames
    /// "module:name" separated by ';', then the path's exclusive nanoseconds.
    std::string folded() const;

private:
    friend struct Stack;
    friend struct Debugger;
    friend struct ::RunVisitor;

    // Called by the interpreter while a section runs.
    // A wasm instruction of the function on top is about to run.
    void on_instr(Opcode::opcode_t opcode){
        ++current_instrs[slot(opcode)];
    }
    // A frame for `funcaddr` was pushed.
    void on_enter(index_t funcaddr);
    // The top frame was popped.
    void on_exit();
    // Stack::run() starts: a new section; frames already on the Stack enter now.
    void begin_stack(const Stack& stack);
    // Stack::run() ends, however it ends: the frames still open exit now.
    void end_stack() noexcept;
    // Around a Debugger stop: the time in between is not counted.
    void pause();
    void resume();

    // Opcodes are 0x00..0xFF plus the 0xFB and 0xFC prefixed ones
    // (<opcode.hpp>); each gets a slot of its own.
    static constexpr size_t slot_count = 3 * 256;
    static size_t slot(Opcode::opcode_t opcode){
        const unsigned prefix = opcode >> 8;
        return (prefix == 0) ? opcode : (((prefix == 0xFB) ? 256 : 512) + (opcode & 0xFF));
    }

    struct FuncStats {
        bool host = false;
        uint64_t calls = 0;
        int64_t inclusive = 0;
        int64_t exclusive = 0;
        std::vector<uint64_t> instrs; // by slot; empty for host functions
        size_t active = 0;            // calls of it currently on the stack
    };
    struct Node {                     // a call path, for the folded output
        index_t funcaddr = 0;
        int64_t exclusive = 0;
        std::map<index_t, std::unique_ptr<Node>> children;
    };
    struct Entry {                    // an open frame
        FuncStats* stats;
        Node* node;
        int64_t start;
        int64_t children;             // inclusive time of its finished callees
    };
    struct Name {
        std::string module;
        std::string func;
    };
    struct Section {
        const Store* store = nullptr;
        std::string entry;
        std::map<index_t, FuncStats> funcs;
        Node root;
        std::vector<Entry> frames;
        index_t entry_funcaddr = 0;
        bool has_entry = false;
        // Filled when the section ends, while the Store is still alive.
        std::map<index_t, Name> names;
        std::map<size_t, std::string> mnemonics; // by slot
    };

    int64_t now() const;
    int64_t raw_now() const;
    void update_current();
    void resolve(Section& section) const;
    Name name_of(const Store& store, index_t funcaddr) const;

    clock_t clock;
    int64_t paused_total = 0;
    int64_t pause_start = 0;
    size_t pause_depth = 0;
    std::map<const ModuleInst*, std::string> module_names;
    std::vector<std::unique_ptr<Section>> sections;
    std::vector<Section*> running; // innermost last
    // The instruction counters of the function on top; a sink when no wasm
    // function is on top, so on_instr needs no check.
    uint64_t* current_instrs;
    std::vector<uint64_t> sink;
};

}

#endif // WASMVM_PROFILE

#endif
