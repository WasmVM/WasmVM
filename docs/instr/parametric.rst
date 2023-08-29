Parametric instructions
=======================

.. cpp:type:: Drop = WasmVM::Instr::Atomic<Opcode::Drop>
.. cpp:enumerator:: Opcode::Drop = 0x1A

.. cpp:struct:: WasmVM::Instr::Select : public Base

    .. cpp:member:: std::vector<ValueType> valtypes

.. cpp:enumerator:: Opcode::Select = 0x1B
.. cpp:enumerator:: Opcode::Select_t = 0x1C

    Select_t appears while selecting with value types 