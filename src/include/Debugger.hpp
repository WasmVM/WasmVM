#ifndef WASMVM_PP_DEBUGGER_DEF
#define WASMVM_PP_DEBUGGER_DEF

// Debugger Layer 1: breakpoints, stepping and stops inside Stack::run().
// Compiled only under WASMVM_DEBUG; without it this header declares nothing.

#ifdef WASMVM_DEBUG

#include <Types.hpp>
#include <exception.hpp>

#include <atomic>
#include <compare>
#include <cstddef>
#include <mutex>
#include <set>

namespace WasmVM {

struct Stack;

/// Attach to a Stack (or module_instanciate) to stop the guest between wasm
/// instructions. This class keeps the breakpoint set, the step state and the
/// pause flag, and decides when to stop; a stop calls on_stop() on the VM
/// thread, which blocks there until it returns. No threads are created here.
///
/// set_breakpoint / clear_breakpoint / clear_breakpoints / pause may be called
/// from any thread, including while the VM thread is blocked in on_stop().
/// One Debugger drives one running Stack at a time.
struct Debugger {
    /// An instruction: its function's store address and its index in
    /// WasmFunc::body (the trailing `end` included). instr == (index_t)-1
    /// means no instruction position (see Stop::position).
    struct Position {
        index_t funcaddr;
        index_t instr;
        auto operator<=>(const Position&) const = default;
    };
    enum class Reason {
        Breakpoint, ///< the instruction about to run has a breakpoint
        Step,       ///< a Next / StepIn / StepOut requested at the last stop completed
        Pause,      ///< pause() was called
        Trap,       ///< the guest trapped; run() rethrows once on_stop returns
        Exception,  ///< a wasm `throw` no try_table catches; run() throws Exception::Uncaught once on_stop returns
    };
    /// How to continue after a stop. Ignored for Reason::Trap and Reason::Exception.
    /// A requested step outlives the run() it was requested in: if that Stack
    /// finishes first, it carries over to the next Stack run with this Debugger.
    enum class Resume {
        Continue, ///< run until a breakpoint, pause, trap or uncaught exception
        Next,     ///< stop at the next instruction in this frame or a caller
        StepIn,   ///< stop at the next wasm instruction, entering calls
        StepOut,  ///< stop at the next instruction in a caller
    };
    struct Stop {
        Reason reason;
        /// Breakpoint / Step / Pause: the instruction about to run.
        /// Trap / Exception: the instruction that raised it; for a trap thrown
        /// by a host function, the call instruction that called it, or, when
        /// no wasm frame called it, {that host function's funcaddr, (index_t)-1}.
        Position position;
        /// The whole stack, frames intact; walk it read-only.
        const Stack& stack;
        /// Reason::Trap only, otherwise nullptr.
        const Exception::Trap* trap;
        /// Reason::Exception only, otherwise null.
        GcPtr<WasmException> exn;
    };

    virtual ~Debugger();
    /// Called on the VM thread at every stop; the guest stays stopped until it returns.
    virtual Resume on_stop(const Stop& stop) = 0;

    void set_breakpoint(Position position);
    void clear_breakpoint(Position position);
    void clear_breakpoints();
    /// Stop before the next wasm instruction. Separate from Stack's interrupt flag.
    void pause();

private:
    friend struct Stack;
    // Called by Stack::run() on the VM thread.
    void before_instr(const Stack& stack, Position position);
    void trapped(const Stack& stack, Position position, const Exception::Trap& trap);
    void uncaught(const Stack& stack, Position position, GcPtr<WasmException> exn);

    std::mutex breakpoints_mutex;
    std::set<Position> breakpoints;
    std::atomic<bool> paused {false};
    // Step state, touched only on the VM thread.
    Resume step = Resume::Continue;
    size_t step_depth = 0; // frame depth at the stop that requested the step
};

}

#endif // WASMVM_DEBUG

#endif
