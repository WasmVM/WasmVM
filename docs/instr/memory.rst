Memory instructions
=====================

Instructions
------------

.. cpp:type:: I32_load = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_load>
.. cpp:enumerator:: WasmVM::Opcode::I32_load = 0x28

.. cpp:type:: I64_load = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load>
.. cpp:enumerator:: WasmVM::Opcode::I64_load = 0x29

.. cpp:type:: F32_load = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::F32_load>
.. cpp:enumerator:: WasmVM::Opcode::F32_load = 0x2A

.. cpp:type:: F64_load = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::F64_load>
.. cpp:enumerator:: WasmVM::Opcode::F64_load = 0x2B

.. cpp:type:: I32_load8_s = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_load8_s>
.. cpp:enumerator:: WasmVM::Opcode::I32_load8_s = 0x2C

.. cpp:type:: I32_load8_u = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_load8_u>
.. cpp:enumerator:: WasmVM::Opcode::I32_load8_u = 0x2D

.. cpp:type:: I32_load16_s = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_load16_s>
.. cpp:enumerator:: WasmVM::Opcode::I32_load16_s = 0x2E

.. cpp:type:: I32_load16_u = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_load16_u>
.. cpp:enumerator:: WasmVM::Opcode::I32_load16_u = 0x2F

.. cpp:type:: I64_load8_s = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load8_s>
.. cpp:enumerator:: WasmVM::Opcode::I64_load8_s = 0x30

.. cpp:type:: I64_load8_u = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load8_u>
.. cpp:enumerator:: WasmVM::Opcode::I64_load8_u = 0x31

.. cpp:type:: I64_load16_s = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load16_s>
.. cpp:enumerator:: WasmVM::Opcode::I64_load16_s = 0x32

.. cpp:type:: I64_load16_u = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load16_u>
.. cpp:enumerator:: WasmVM::Opcode::I64_load16_u = 0x33

.. cpp:type:: I64_load32_s = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load32_s>
.. cpp:enumerator:: WasmVM::Opcode::I64_load32_s = 0x34

.. cpp:type:: I64_load32_u = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_load32_u>
.. cpp:enumerator:: WasmVM::Opcode::I64_load32_u = 0x35

.. cpp:type:: I32_store = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_store>
.. cpp:enumerator:: WasmVM::Opcode::I32_store = 0x36

.. cpp:type:: I64_store = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_store>
.. cpp:enumerator:: WasmVM::Opcode::I64_store = 0x37

.. cpp:type:: F32_store = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::F32_store>
.. cpp:enumerator:: WasmVM::Opcode::F32_store = 0x38

.. cpp:type:: F64_store = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::F64_store>
.. cpp:enumerator:: WasmVM::Opcode::F64_store = 0x39

.. cpp:type:: I32_store8 = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_store8>
.. cpp:enumerator:: WasmVM::Opcode::I32_store8 = 0x3A

.. cpp:type:: I32_store16 = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I32_store16>
.. cpp:enumerator:: WasmVM::Opcode::I32_store16 = 0x3B

.. cpp:type:: I64_store8 = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_store8>
.. cpp:enumerator:: WasmVM::Opcode::I64_store8 = 0x3C

.. cpp:type:: I64_store16 = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_store16>
.. cpp:enumerator:: WasmVM::Opcode::I64_store16 = 0x3D

.. cpp:type:: I64_store32 = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::I64_store32>
.. cpp:enumerator:: WasmVM::Opcode::I64_store32 = 0x3E

.. cpp:type:: Memory_size = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::Memory_size>
.. cpp:enumerator:: WasmVM::Opcode::Memory_size = 0x3F

.. cpp:type:: Memory_grow = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::Memory_grow>
.. cpp:enumerator:: WasmVM::Opcode::Memory_grow = 0x40

.. cpp:struct:: Memory_init : public Base

    .. cpp:function:: Memory_init(index_t memidx, index_t dataidx)

    .. cpp:member:: index_t memidx = 0

    .. cpp:member:: index_t dataidx = 0

.. cpp:enumerator:: WasmVM::Opcode::Memory_init = 0xFC08

.. cpp:type:: WasmVM::Instr::Data_drop = WasmVM::Instr::OneIndex<WasmVM::Opcode::Data_drop>
.. cpp:enumerator:: WasmVM::Opcode::Data_drop = 0xFC09

.. cpp:struct:: WasmVM::Instr::Memory_copy : public Base

    .. cpp:function:: Memory_copy(index_t dstidx, index_t srcidx)

    .. cpp:member:: index_t dstidx = 0

    .. cpp:member:: index_t srcidx = 0

.. cpp:enumerator:: WasmVM::Opcode::Memory_copy = 0xFC0A

.. cpp:type:: Memory_fill = WasmVM::Instr::MemoryInstr::Class<WasmVM::Opcode::Memory_fill>
.. cpp:enumerator:: WasmVM::Opcode::Memory_fill = 0xFC0B

Definitions & Templates
-----------------------

.. cpp:struct:: WasmVM::Instr::MemoryInstr::Base : public Instr::Base

    .. cpp:function:: Base(WasmVM::Opcode::Opcode op, index_t memidx, offset_t offset, align_t align)

    .. cpp:member:: index_t memidx
    .. cpp:member:: offset_t offset
    .. cpp:member:: align_t align

.. cpp:struct:: template<WasmVM::Opcode::Opcode OP> WasmVM::Instr::MemoryInstr::Class : public MemoryInstr::Base

    .. cpp:function:: Class(index_t memidx, offset_t offset, align_t align)
