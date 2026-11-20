API Functions
=============

Function
--------

.. cpp:function:: WasmModule WasmVM::module_parse(std::istream& stream)
.. cpp:function:: std::ostream& WasmVM::module_dump(const WasmModule& module, std::ostream& stream)
.. cpp:function:: std::ostream& WasmVM::module_encode(const WasmModule& module, std::ostream& stream)
.. cpp:function:: WasmModule WasmVM::module_decode(std::istream& stream)
.. cpp:function:: std::optional<Exception::Exception> WasmVM::module_validate(const WasmModule& module)
.. cpp:function:: ModuleInst& WasmVM::module_instanciate(ModuleInst& moduleInst, Store& store, const WasmModule& module, std::vector<ExternVal> externval, Observers observers = {})

    ``observers``: the debugger and profiler the start function runs under,
    if any; see :cpp:struct:`WasmVM::Observers`.

Exception
---------

.. cpp:struct:: WasmVM::Exception::Exception : public std::runtime_error

    .. cpp:function:: Exception(std::string msg)

.. cpp:struct:: WasmVM::Exception::Parse : public Exception

    .. cpp:function:: Parse(std::string msg, std::pair<size_t, size_t> location)
    .. cpp:member:: std::pair<size_t, size_t> location

.. cpp:struct:: WasmVM::Exception::Decode : public Exception

    .. cpp:function:: Decode(std::string msg, size_t location)
    .. cpp:member:: size_t location

.. cpp:struct:: WasmVM::Exception::unexpected_eof : public Exception

    .. cpp:function:: unexpected_eof()

.. cpp:struct:: WasmVM::Exception::interrupted : public Exception

    Thrown by ``Stack::run()`` when the embedder's interrupt flag reads true.
    Not a wasm-level exception: the guest's ``try_table``/``catch_all`` cannot
    swallow it.

    .. cpp:function:: interrupted()

.. cpp:struct:: WasmVM::Exception::Trap : public Exception

    Base of every WebAssembly trap escaping ``Stack::run()``: the guest hit a
    condition the spec defines as trapping (``unreachable``, integer divide by
    zero, out-of-bounds memory or table access, indirect call type mismatch,
    null reference, failed cast, ...). The concrete trap types are private to
    WasmVM; catch this base.

    .. cpp:function:: Trap(std::string msg)

.. cpp:struct:: WasmVM::Exception::Uncaught : public Exception

    A wasm ``throw`` no ``try_table`` in the guest caught, escaping
    ``Stack::run()``. Not a ``Trap``.

    .. cpp:function:: Uncaught(std::shared_ptr<WasmException> exn)
    .. cpp:member:: std::shared_ptr<WasmException> exn

        The thrown exception: its tag address and payload values.

.. cpp:struct:: WasmVM::Exception::Warning

    .. cpp:function:: Warning(std::string msg)
    .. cpp:function:: Warning(const char* msg)
    .. cpp:member:: static void regist(std::function<void(std::string)> handler)

Debugger
--------

Declared in ``Debugger.hpp`` and only in a build with ``WASMVM_DEBUG``.
``wasmvm --dap`` is built on it.

.. cpp:struct:: WasmVM::Debugger

    Attach to a :cpp:struct:`WasmVM::Stack` or to ``module_instanciate``
    through :cpp:member:`WasmVM::Observers::debugger` to stop the guest
    between wasm instructions. It keeps the breakpoints, the step state and the
    pause flag, and decides when to stop. A stop calls :cpp:func:`on_stop` on
    the thread running the guest, which stays blocked until it returns. No
    threads are created. One Debugger drives one running Stack at a time.

    :cpp:func:`set_breakpoint`, :cpp:func:`clear_breakpoint`,
    :cpp:func:`clear_breakpoints` and :cpp:func:`pause` may be called from any
    thread, also while :cpp:func:`on_stop` is running.

    .. cpp:function:: virtual Resume on_stop(const Stop& stop) = 0

        Called at every stop; the guest stays stopped until it returns. The
        return value says how to go on, and is ignored for
        :cpp:enumerator:`Reason::Trap` and :cpp:enumerator:`Reason::Exception`.

    .. cpp:function:: void set_breakpoint(Position position)
    .. cpp:function:: void clear_breakpoint(Position position)
    .. cpp:function:: void clear_breakpoints()
    .. cpp:function:: void pause()

        Stop before the next wasm instruction. Separate from the Stack's
        interrupt flag.

    .. cpp:struct:: Position

        An instruction: its function's store address and its index in
        ``WasmFunc::body``, the trailing ``end`` included. ``instr ==
        (index_t)-1`` means no instruction position.

        .. cpp:member:: index_t funcaddr
        .. cpp:member:: index_t instr

    .. cpp:enum-class:: Reason

        .. cpp:enumerator:: Breakpoint

            The instruction about to run has a breakpoint.

        .. cpp:enumerator:: Step

            A step requested at the last stop completed.

        .. cpp:enumerator:: Pause

            :cpp:func:`pause` was called.

        .. cpp:enumerator:: Trap

            The guest trapped. ``run()`` rethrows once :cpp:func:`on_stop`
            returns.

        .. cpp:enumerator:: Exception

            A wasm ``throw`` no ``try_table`` catches. ``run()`` throws
            :cpp:struct:`WasmVM::Exception::Uncaught` once :cpp:func:`on_stop`
            returns.

    .. cpp:enum-class:: Resume

        Steps are by frame depth and stop only between wasm instructions, so a
        host function call is stepped over as one unit. A requested step
        outlives the ``run()`` it was requested in: if that Stack finishes
        first, it carries over to the next Stack run with this Debugger.

        .. cpp:enumerator:: Continue

            Run until a breakpoint, a pause, a trap or an uncaught exception.

        .. cpp:enumerator:: Next

            Stop at the next instruction in this frame or a caller.

        .. cpp:enumerator:: StepIn

            Stop at the next wasm instruction, entering calls.

        .. cpp:enumerator:: StepOut

            Stop at the next instruction in a caller.

    .. cpp:struct:: Stop

        .. cpp:member:: Reason reason
        .. cpp:member:: Position position

            For a breakpoint, a step or a pause, the instruction about to run.
            For a trap or an exception, the instruction that raised it; for a
            trap thrown by a host function, the call instruction that called
            it, or ``{host funcaddr, (index_t)-1}`` when no wasm frame called
            it.

        .. cpp:member:: const Stack& stack

            The whole stack with its frames intact, to be walked read-only.

        .. cpp:member:: const Exception::Trap* trap

            :cpp:enumerator:`Reason::Trap` only, otherwise ``nullptr``.

        .. cpp:member:: GcPtr<WasmException> exn

            :cpp:enumerator:`Reason::Exception` only, otherwise null.

Profiler
--------

Declared in ``Profiler.hpp`` and only in a build with ``WASMVM_PROFILE``.
``wasmvm --profile`` is built on it; the report formats are described in
:doc:`programs/vm`.

.. cpp:struct:: WasmVM::Profiler

    Attach to a :cpp:struct:`WasmVM::Stack` or to ``module_instanciate``
    through :cpp:member:`WasmVM::Observers::profiler` to record what the guest
    runs: instruction counts per function and opcode, call counts, and
    inclusive and exclusive time. It never stops the VM.

    Every ``Stack::run()`` is one section of the report. A ``run()`` started
    while another one on the same Profiler is still running gets a section of
    its own; its time still counts for the outer section's open frames.

    Time is read only on function entry and exit. Time a
    :cpp:struct:`WasmVM::Debugger` attached to the same Stack spends stopped
    is not counted. A function's inclusive time counts only its outermost
    active call; every call adds to its call count and its exclusive time.

    Not thread-safe: one thread drives it.

    .. cpp:type:: clock_t = int64_t (*)()

        A clock in nanoseconds.

    .. cpp:function:: explicit Profiler(clock_t now = nullptr)

        ``nullptr`` means ``std::chrono::steady_clock``.

    .. cpp:function:: void set_module_name(const ModuleInst& module, std::string name)

        The name the report gives a module's functions. Without one, the
        module name from the name section is used, and with neither the module
        name is empty.

    .. cpp:function:: void finish()

        Ends every section still running at this moment: their open frames
        exit now. For a program that ends without returning from ``run()``,
        e.g. ``std::exit`` called inside a host function.

    .. cpp:function:: Json::Value report() const

        The full report, schema version 1.

    .. cpp:function:: std::string folded() const

        Collapsed stacks for flame graphs: one line per call path, frames
        ``module:name`` separated by ``;``, then the path's exclusive
        nanoseconds.
