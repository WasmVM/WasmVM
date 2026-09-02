API Functions
=============

Function
--------

.. cpp:function:: WasmModule WasmVM::module_parse(std::istream& stream)
.. cpp:function:: std::ostream& WasmVM::module_dump(const WasmModule& module, std::ostream& stream)
.. cpp:function:: std::ostream& WasmVM::module_encode(const WasmModule& module, std::ostream& stream)
.. cpp:function:: WasmModule WasmVM::module_decode(std::istream& stream)
.. cpp:function:: std::optional<Exception::Exception> WasmVM::module_validate(const WasmModule& module)
.. cpp:function:: ModuleInst& WasmVM::module_instanciate(ModuleInst& moduleInst, Store& store, const WasmModule& module, std::vector<ExternVal> externval)

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