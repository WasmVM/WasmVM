Reference instructions
======================

.. cpp:struct:: WasmVM::Instr::Ref_null : public Base

    .. cpp:function:: Ref_null(RefType heaptype)

    .. cpp:member:: RefType heaptype = RefType::funcref

.. cpp:enumerator:: WasmVM::Opcode::Ref_null = 0xD0

.. cpp:type:: WasmVM::Instr::Ref_is_null = WasmVM::Instr::Atomic<WasmVM::Opcode::Ref_is_null>
.. cpp:enumerator:: WasmVM::Opcode::Ref_is_null = 0xD1

.. cpp:type:: WasmVM::Instr::Ref_func = WasmVM::Instr::OneIndex<WasmVM::Opcode::Ref_func>
.. cpp:enumerator:: WasmVM::Opcode::Ref_func = 0xD2