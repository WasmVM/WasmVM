Structure
=========

Data structures of WasmVM objects for general use.

.. cpp:struct:: WasmFunc

    Function in WebAssembly

    .. cpp:member:: index_t typeidx

        Function type index

    .. cpp:member:: std::vector<ValueType> locals

        Local variables

    .. cpp:member:: std::vector<WasmInstr> body

        Function body

.. cpp:struct:: WasmGlobal

    .. cpp:member:: GlobalType type

    .. cpp:member:: ConstInstr init

.. cpp:struct:: WasmElem

    .. cpp:member:: RefType type

    .. cpp:member:: std::vector<ConstInstr> elemlist
        
    .. cpp:member:: ElemMode mode

    .. cpp:struct:: ElemMode

        .. cpp:enum:: Mode

            .. cpp:enumerator:: passive

            .. cpp:enumerator:: active

            .. cpp:enumerator:: declarative

        .. cpp:member:: Mode type

        .. cpp:member:: std::optional<index_t> tableidx

        .. cpp:member:: std::optional<ConstInstr> offset

.. cpp:struct:: WasmData

    .. cpp:member:: std::vector<byte_t> init
        
    .. cpp:member:: DataMode mode

    .. cpp:struct:: DataMode

        .. cpp:enum:: Mode

            .. cpp:enumerator:: passive

            .. cpp:enumerator:: active

        .. cpp:member:: Mode type

        .. cpp:member:: std::optional<index_t> memidx

        .. cpp:member:: std::optional<ConstInstr> offset

.. cpp:struct:: WasmExport

    .. cpp:enum:: DescType

            .. cpp:enumerator:: func = 0x00

            .. cpp:enumerator:: table = 0x01

            .. cpp:enumerator:: mem = 0x02

            .. cpp:enumerator:: global = 0x03
    
    .. cpp:member:: std::string name

    .. cpp:member:: DescType desc

    .. cpp:member:: index_t index

.. cpp:struct:: WasmImport
    
    .. cpp:member:: std::string module

    .. cpp:member:: std::string name

    .. cpp:member:: std::variant<index_t, TableType, MemType, GlobalType> desc