Variable instructions
=====================

.. cpp:type:: WasmVM::Instr::Local_get = WasmVM::Instr::OneIndex<WasmVM::Opcode::Local_get>
.. cpp:enumerator:: WasmVM::Opcode::Local_get = 0x20

.. cpp:type:: WasmVM::Instr::Local_set = WasmVM::Instr::OneIndex<WasmVM::Opcode::Local_set>
.. cpp:enumerator:: WasmVM::Opcode::Local_set = 0x21

.. cpp:type:: WasmVM::Instr::Local_tee = WasmVM::Instr::OneIndex<WasmVM::Opcode::Local_tee>
.. cpp:enumerator:: WasmVM::Opcode::Local_tee = 0x22

.. cpp:type:: WasmVM::Instr::Global_get = WasmVM::Instr::OneIndex<WasmVM::Opcode::Global_get>
.. cpp:enumerator:: WasmVM::Opcode::Global_get = 0x23

.. cpp:type:: WasmVM::Instr::Global_set = WasmVM::Instr::OneIndex<WasmVM::Opcode::Global_set>
.. cpp:enumerator:: WasmVM::Opcode::Global_set = 0x24