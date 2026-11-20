Instance
========

Stack
-----

.. cpp:struct:: WasmVM::Stack

    .. cpp:function:: Stack(Store& store, const std::atomic<bool>* interrupt = nullptr, Observers observers = {})

        ``interrupt``: an optional flag owned by the embedder; when it reads
        true, ``run()`` throws :cpp:struct:`WasmVM::Exception::interrupted`.
        ``observers``: the debugger and profiler attached to this Stack, if
        any.

    .. cpp:function:: void invoke(index_t funcaddr, std::vector<Value> args)
    .. cpp:function:: std::vector<Value> run()
    .. cpp:member:: Store& store
    .. cpp:member:: const std::atomic<bool>* interrupt
    .. cpp:member:: Observers observers
    .. cpp:member:: Frames frames

        A ``std::stack<Frame>`` that can also be walked read-only:
        ``begin()`` / ``end()`` from bottom to top, ``rbegin()`` / ``rend()``
        from top to bottom. :cpp:member:`Frame::labels` and
        :cpp:member:`Label::values` can be walked the same way.

    .. cpp:member:: std::vector<Value> results

.. cpp:struct:: WasmVM::Observers

    What watches a :cpp:struct:`WasmVM::Stack` while it runs, each owned by
    the embedder. A member exists only in a build with its option; with
    neither, the struct is empty. Code using the installed headers must
    define ``WASMVM_DEBUG`` and ``WASMVM_PROFILE`` as the library was built.

    .. cpp:member:: Debugger* debugger = nullptr

        Only with ``WASMVM_DEBUG``. ``run()`` stops through it between wasm
        instructions; see :cpp:struct:`WasmVM::Debugger`.

    .. cpp:member:: Profiler* profiler = nullptr

        Only with ``WASMVM_PROFILE``. ``run()`` reports every instruction,
        function entry and exit to it, and never stops for it; see
        :cpp:struct:`WasmVM::Profiler`.

.. cpp:struct:: WasmVM::Frame

    Per-call activation frame pushed onto :cpp:member:`Stack::frames`.

    .. cpp:function:: Frame(ModuleInst& module, index_t funcaddr)
    .. cpp:member:: Labels labels
    .. cpp:member:: std::vector<Value> locals
    .. cpp:member:: ModuleInst& module
    .. cpp:member:: index_t funcaddr

.. cpp:struct:: WasmVM::Label

    .. cpp:member:: size_t arity
    .. cpp:member:: std::optional<Counters> pc
    .. cpp:member:: Values values
    .. cpp:member:: std::vector<CatchEntry> catches

        Non-empty for ``try_table`` labels (exception-handling proposal).

.. cpp:struct:: WasmVM::Label::CatchEntry

    A single catch clause of a ``try_table`` label.

    .. cpp:enum:: Kind

        .. cpp:enumerator:: Catch
        .. cpp:enumerator:: CatchRef
        .. cpp:enumerator:: CatchAll
        .. cpp:enumerator:: CatchAllRef

    .. cpp:member:: Kind kind
    .. cpp:member:: index_t tag_addr

        Resolved tag address (only valid for ``Catch`` / ``CatchRef``).

    .. cpp:member:: index_t label_idx

        Branch target label index.

.. cpp:struct:: WasmVM::Label::Values : public std::stack<Value>

    .. cpp:function:: std::vector<Value> get()
    .. cpp:function:: void insert(std::vector<Value>)

.. cpp:struct:: WasmVM::Label::Counters : public std::pair<size_t, size_t>

    .. cpp:member:: size_t& current
    .. cpp:member:: size_t& continuation

Store
-----

.. cpp:struct:: WasmVM::Store

    .. cpp:member:: std::vector<FuncInst> funcs
    .. cpp:member:: std::vector<TableInst> tables
    .. cpp:member:: std::vector<MemInst> mems
    .. cpp:member:: std::vector<GlobalInst> globals
    .. cpp:member:: std::vector<ElemInst> elems
    .. cpp:member:: std::vector<DataInst> datas
    .. cpp:member:: std::vector<TagInst> tags

Instances
---------

.. cpp:type:: WasmVM::hostfunc_t = std::vector<Value>(*)(Stack&)

.. cpp:struct:: WasmVM::ModuleInst

    .. cpp:member:: std::vector<FuncType> types
    .. cpp:member:: std::vector<index_t> funcaddrs
    .. cpp:member:: std::vector<index_t> tableaddrs
    .. cpp:member:: std::vector<index_t> memaddrs
    .. cpp:member:: std::vector<index_t> globaladdrs
    .. cpp:member:: std::vector<index_t> elemaddrs
    .. cpp:member:: std::vector<index_t> dataaddrs
    .. cpp:member:: std::vector<index_t> tagaddrs
    .. cpp:member:: std::vector<ExportInst> exports
    .. cpp:member:: WasmNames names

        Copied from :cpp:member:`WasmVM::WasmModule::names` at instantiation;
        indices are the module's own, not store addresses.

    .. cpp:function:: void reset()

.. cpp:struct:: WasmVM::FuncInst

    .. cpp:function:: FuncInst(ModuleInst& module)
    .. cpp:member:: FuncType type
    .. cpp:member:: ModuleInst& module
    .. cpp:member:: std::variant<hostfunc_t, WasmFunc> body

.. cpp:struct:: WasmVM::TableInst

    .. cpp:member:: TableType type
    .. cpp:member:: std::vector<WasmVM::Ref> elems

.. cpp:struct:: WasmVM::MemInst

    .. cpp:member:: MemType type
    .. cpp:member:: std::vector<byte_t> data

.. cpp:struct:: WasmVM::GlobalInst

    .. cpp:member:: ValueType type
    .. cpp:member:: Value value

.. cpp:struct:: WasmVM::ElemInst

    .. cpp:member:: RefType type
    .. cpp:member:: std::vector<Ref> elem

.. cpp:type:: WasmVM::DataInst = std::vector<byte_t>
.. cpp:struct:: WasmVM::ExportInst

    .. cpp:member:: std::string name
    .. cpp:member:: ExternVal value
