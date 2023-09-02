Instance
========

Stack
-----

.. cpp:struct:: WasmVM::Stack

    .. cpp:function:: Stack(Store& store)
    .. cpp:function:: void invoke(index_t funcaddr, std::vector<Value> args)
    .. cpp:member:: std::stack<Label> labels
    .. cpp:member:: std::vector<Value> locals
    .. cpp:member:: ModuleInst& module
    .. cpp:member:: index_t funcaddr

.. cpp:struct:: WasmVM::Label
    
    .. cpp:member:: size_t arity
    .. cpp:member:: std::optional<Counters> pc
    .. cpp:member:: Values values

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
    .. cpp:member:: std::vector<ExportInst> exports

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
