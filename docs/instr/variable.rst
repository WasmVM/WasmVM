Variable instructions
=====================

.. cpp:type:: Local_get = WasmVM::Instr::OneIndex<Opcode::Local_get>
.. cpp:enumerator:: Opcode::Local_get = 0x20

.. cpp:type:: Local_set = WasmVM::Instr::OneIndex<Opcode::Local_set>
.. cpp:enumerator:: Opcode::Local_set = 0x21

.. cpp:type:: Local_tee = WasmVM::Instr::OneIndex<Opcode::Local_tee>
.. cpp:enumerator:: Opcode::Local_tee = 0x22

.. cpp:type:: Global_get = WasmVM::Instr::OneIndex<Opcode::Global_get>
.. cpp:enumerator:: Opcode::Global_get = 0x23

.. cpp:type:: Global_set = WasmVM::Instr::OneIndex<Opcode::Global_set>
.. cpp:enumerator:: Opcode::Global_set = 0x24