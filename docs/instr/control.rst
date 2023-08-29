Control instructions
=======================

.. cpp:type:: WasmVM::Instr::Unreachable = WasmVM::Instr::Atomic<WasmVM::Opcode::Unreachable>
.. cpp:enumerator:: WasmVM::Opcode::Unreachable = 0x00

.. cpp:type:: WasmVM::Instr::Nop = WasmVM::Instr::Atomic<WasmVM::Opcode::Nop>
.. cpp:enumerator:: WasmVM::Opcode::Nop = 0x01

.. cpp:type:: WasmVM::Instr::Block = WasmVM::Instr::BlockInstr<WasmVM::Opcode::Block>
.. cpp:enumerator:: WasmVM::Opcode::Block = 0x02

.. cpp:type:: WasmVM::Instr::Loop = WasmVM::Instr::BlockInstr<WasmVM::Opcode::Loop>
.. cpp:enumerator:: WasmVM::Opcode::Loop = 0x03

.. cpp:type:: WasmVM::Instr::If = WasmVM::Instr::BlockInstr<WasmVM::Opcode::If>
.. cpp:enumerator:: WasmVM::Opcode::If = 0x04

.. cpp:type:: WasmVM::Instr::Else = WasmVM::Instr::Atomic<WasmVM::Opcode::Else>
.. cpp:enumerator:: WasmVM::Opcode::Else = 0x05

.. cpp:type:: WasmVM::Instr::End = WasmVM::Instr::Atomic<WasmVM::Opcode::End>
.. cpp:enumerator:: WasmVM::Opcode::End = 0x0B

.. cpp:type:: WasmVM::Instr::Br = WasmVM::Instr::OneIndex<WasmVM::Opcode::Br>
.. cpp:enumerator:: WasmVM::Opcode::Br = 0x0C

.. cpp:type:: WasmVM::Instr::Br_if = WasmVM::Instr::OneIndex<WasmVM::Opcode::Br_if>
.. cpp:enumerator:: WasmVM::Opcode::Br_if = 0x0D

.. cpp:struct:: WasmVM::Instr::Br_table : public Base

    .. cpp:member:: std::vector<index_t> indices

.. cpp:enumerator:: WasmVM::Opcode::Br_table = 0x0E

.. cpp:type:: WasmVM::Instr::Return = WasmVM::Instr::Atomic<WasmVM::Opcode::Return>
.. cpp:enumerator:: WasmVM::Opcode::Return = 0x0F

.. cpp:type:: WasmVM::Instr::Call = WasmVM::Instr::OneIndex<WasmVM::Opcode::Call>
.. cpp:enumerator:: WasmVM::Opcode::Call = 0x10

.. cpp:struct:: WasmVM::Instr::Call_indirect : public Base

    .. cpp:function:: Call_indirect(index_t tableidx, index_t typeidx)
    .. cpp:member:: index_t tableidx
    .. cpp:member:: index_t typeidx

.. cpp:enumerator:: WasmVM::Opcode::Call_indirect = 0x11