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

Tail-call instructions
----------------------

.. cpp:type:: WasmVM::Instr::Return_call = WasmVM::Instr::OneIndex<WasmVM::Opcode::Return_call>
.. cpp:enumerator:: WasmVM::Opcode::Return_call = 0x12

.. cpp:struct:: WasmVM::Instr::Return_call_indirect : public Base

    .. cpp:function:: Return_call_indirect(index_t tableidx, index_t typeidx)
    .. cpp:member:: index_t tableidx
    .. cpp:member:: index_t typeidx

.. cpp:enumerator:: WasmVM::Opcode::Return_call_indirect = 0x13

.. cpp:type:: WasmVM::Instr::Return_call_ref = WasmVM::Instr::OneIndex<WasmVM::Opcode::Return_call_ref>
.. cpp:enumerator:: WasmVM::Opcode::Return_call_ref = 0x15

.. cpp:type:: WasmVM::Instr::Call_ref = WasmVM::Instr::OneIndex<WasmVM::Opcode::Call_ref>
.. cpp:enumerator:: WasmVM::Opcode::Call_ref = 0x14

Exception handling instructions
-------------------------------

.. cpp:type:: WasmVM::Instr::Throw = WasmVM::Instr::OneIndex<WasmVM::Opcode::Throw>
.. cpp:enumerator:: WasmVM::Opcode::Throw = 0x08

.. cpp:type:: WasmVM::Instr::Throw_ref = WasmVM::Instr::Atomic<WasmVM::Opcode::Throw_ref>
.. cpp:enumerator:: WasmVM::Opcode::Throw_ref = 0x0A

.. cpp:struct:: WasmVM::Instr::TryCatchEntry

    A single ``catch`` clause inside a :cpp:struct:`Try_table`.

    .. cpp:enum:: Kind

        .. cpp:enumerator:: Catch = 0
        .. cpp:enumerator:: CatchRef = 1
        .. cpp:enumerator:: CatchAll = 2
        .. cpp:enumerator:: CatchAllRef = 3

    .. cpp:member:: Kind kind
    .. cpp:member:: index_t tag_idx

        Tag index. Only used for ``Catch`` and ``CatchRef``.

    .. cpp:member:: index_t label_idx

.. cpp:struct:: WasmVM::Instr::Try_table : public Base

    .. cpp:member:: std::optional<index_t> type
    .. cpp:member:: std::vector<TryCatchEntry> catches

.. cpp:enumerator:: WasmVM::Opcode::Try_table = 0x1F