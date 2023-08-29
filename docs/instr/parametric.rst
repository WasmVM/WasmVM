Parametric instructions
=======================

.. cpp:type:: WasmVM::Instr::Drop = WasmVM::Instr::Atomic<WasmVM::Opcode::Drop>
.. cpp:enumerator:: WasmVM::Opcode::Drop = 0x1A

.. cpp:struct:: WasmVM::Instr::Select : public Base

    .. cpp:member:: std::vector<ValueType> valtypes

.. cpp:enumerator:: WasmVM::Opcode::Select = 0x1B
.. cpp:enumerator:: WasmVM::Opcode::Select_t = 0x1C

    Select_t appears while selecting with value types 