Table instructions
==================

.. cpp:type:: WasmVM::Instr::Table_get = WasmVM::Instr::OneIndex<WasmVM::Opcode::Table_get>
.. cpp:enumerator:: WasmVM::Opcode::Table_get = 0x25

.. cpp:type:: WasmVM::Instr::Table_set = WasmVM::Instr::OneIndex<WasmVM::Opcode::Table_set>
.. cpp:enumerator:: WasmVM::Opcode::Table_set = 0x26

.. cpp:struct:: WasmVM::Instr::Table_init : public Base

    .. cpp:function:: Table_init(index_t tableidx, index_t elemidx)

    .. cpp:member:: index_t tableidx = 0

    .. cpp:member:: index_t elemidx = 0

.. cpp:enumerator:: WasmVM::Opcode::Table_init = 0xFC0C

.. cpp:type:: WasmVM::Instr::Elem_drop = WasmVM::Instr::OneIndex<WasmVM::Opcode::Elem_drop>
.. cpp:enumerator:: WasmVM::Opcode::Elem_drop = 0xFC0D

.. cpp:type:: WasmVM::Instr::Table_grow = WasmVM::Instr::OneIndex<WasmVM::Opcode::Table_grow>
.. cpp:enumerator:: WasmVM::Opcode::Table_grow = 0xFC0E

.. cpp:struct:: WasmVM::Instr::Table_copy : public Base

    .. cpp:function:: Table_copy(index_t dstidx, index_t srcidx)

    .. cpp:member:: index_t dstidx = 0

    .. cpp:member:: index_t srcidx = 0

.. cpp:enumerator:: WasmVM::Opcode::Table_copy = 0xFC0F

.. cpp:type:: WasmVM::Instr::Table_size = WasmVM::Instr::OneIndex<WasmVM::Opcode::Table_size>
.. cpp:enumerator:: WasmVM::Opcode::Table_size = 0xFC10

.. cpp:type:: WasmVM::Instr::Table_fill = WasmVM::Instr::OneIndex<WasmVM::Opcode::Table_fill>
.. cpp:enumerator:: WasmVM::Opcode::Table_fill = 0xFC11

