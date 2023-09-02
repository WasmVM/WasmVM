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

.. cpp:struct:: WasmVM::Exception::Warning

    .. cpp:function:: Warning(std::string msg)
    .. cpp:function:: Warning(const char* msg)
    .. cpp:member:: static void regist(std::function<void(std::string)> handler)