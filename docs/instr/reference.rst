Reference instructions
======================

.. cpp:struct:: WasmVM::Instr::Ref_null : public Base

    .. cpp:function:: Ref_null(RefType heaptype)

    .. cpp:member:: RefType heaptype = RefType::funcref

.. cpp:enumerator:: Opcode::Ref_null = 0xD0

.. cpp:type:: Ref_is_null = WasmVM::Instr::Atomic<Opcode::Ref_is_null>
.. cpp:enumerator:: Opcode::Ref_is_null = 0xD1

.. cpp:type:: Ref_func = WasmVM::Instr::OneIndex<Opcode::Ref_func>
.. cpp:enumerator:: Opcode::Ref_func = 0xD2