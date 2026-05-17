GC instructions
===============

Aggregate (struct and array) instructions from the WebAssembly GC proposal.
All opcodes are encoded with the ``0xFB`` prefix.

Struct instructions
-------------------

.. cpp:type:: WasmVM::Instr::Struct_new = WasmVM::Instr::OneIndex<WasmVM::Opcode::Struct_new>
.. cpp:enumerator:: WasmVM::Opcode::Struct_new = 0xFB00

.. cpp:type:: WasmVM::Instr::Struct_new_default = WasmVM::Instr::OneIndex<WasmVM::Opcode::Struct_new_default>
.. cpp:enumerator:: WasmVM::Opcode::Struct_new_default = 0xFB01

.. cpp:struct:: WasmVM::Instr::Struct_get : public Base

    .. cpp:function:: Struct_get(index_t typeidx, index_t fieldidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t fieldidx

.. cpp:enumerator:: WasmVM::Opcode::Struct_get = 0xFB02

.. cpp:struct:: WasmVM::Instr::Struct_get_s : public Base

    .. cpp:function:: Struct_get_s(index_t typeidx, index_t fieldidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t fieldidx

.. cpp:enumerator:: WasmVM::Opcode::Struct_get_s = 0xFB03

.. cpp:struct:: WasmVM::Instr::Struct_get_u : public Base

    .. cpp:function:: Struct_get_u(index_t typeidx, index_t fieldidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t fieldidx

.. cpp:enumerator:: WasmVM::Opcode::Struct_get_u = 0xFB04

.. cpp:struct:: WasmVM::Instr::Struct_set : public Base

    .. cpp:function:: Struct_set(index_t typeidx, index_t fieldidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t fieldidx

.. cpp:enumerator:: WasmVM::Opcode::Struct_set = 0xFB05

Array instructions
------------------

.. cpp:type:: WasmVM::Instr::Array_new = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_new>
.. cpp:enumerator:: WasmVM::Opcode::Array_new = 0xFB06

.. cpp:type:: WasmVM::Instr::Array_new_default = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_new_default>
.. cpp:enumerator:: WasmVM::Opcode::Array_new_default = 0xFB07

.. cpp:struct:: WasmVM::Instr::Array_new_fixed : public Base

    .. cpp:function:: Array_new_fixed(index_t typeidx, u32_t n)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: u32_t n

.. cpp:enumerator:: WasmVM::Opcode::Array_new_fixed = 0xFB08

.. cpp:struct:: WasmVM::Instr::Array_new_data : public Base

    .. cpp:function:: Array_new_data(index_t typeidx, index_t dataidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t dataidx

.. cpp:enumerator:: WasmVM::Opcode::Array_new_data = 0xFB09

.. cpp:struct:: WasmVM::Instr::Array_new_elem : public Base

    .. cpp:function:: Array_new_elem(index_t typeidx, index_t elemidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t elemidx

.. cpp:enumerator:: WasmVM::Opcode::Array_new_elem = 0xFB0A

.. cpp:type:: WasmVM::Instr::Array_get = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_get>
.. cpp:enumerator:: WasmVM::Opcode::Array_get = 0xFB0B

.. cpp:type:: WasmVM::Instr::Array_get_s = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_get_s>
.. cpp:enumerator:: WasmVM::Opcode::Array_get_s = 0xFB0C

.. cpp:type:: WasmVM::Instr::Array_get_u = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_get_u>
.. cpp:enumerator:: WasmVM::Opcode::Array_get_u = 0xFB0D

.. cpp:type:: WasmVM::Instr::Array_set = WasmVM::Instr::OneIndex<WasmVM::Opcode::Array_set>
.. cpp:enumerator:: WasmVM::Opcode::Array_set = 0xFB0E

.. cpp:type:: WasmVM::Instr::Array_len = WasmVM::Instr::Atomic<WasmVM::Opcode::Array_len>
.. cpp:enumerator:: WasmVM::Opcode::Array_len = 0xFB0F

.. cpp:struct:: WasmVM::Instr::Array_fill : public Base

    .. cpp:function:: Array_fill(index_t typeidx)
    .. cpp:member:: index_t typeidx

.. cpp:enumerator:: WasmVM::Opcode::Array_fill = 0xFB10

.. cpp:struct:: WasmVM::Instr::Array_copy : public Base

    .. cpp:function:: Array_copy(index_t dst_typeidx, index_t src_typeidx)
    .. cpp:member:: index_t dst_typeidx
    .. cpp:member:: index_t src_typeidx

.. cpp:enumerator:: WasmVM::Opcode::Array_copy = 0xFB11

.. cpp:struct:: WasmVM::Instr::Array_init_data : public Base

    .. cpp:function:: Array_init_data(index_t typeidx, index_t dataidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t dataidx

.. cpp:enumerator:: WasmVM::Opcode::Array_init_data = 0xFB12

.. cpp:struct:: WasmVM::Instr::Array_init_elem : public Base

    .. cpp:function:: Array_init_elem(index_t typeidx, index_t elemidx)
    .. cpp:member:: index_t typeidx
    .. cpp:member:: index_t elemidx

.. cpp:enumerator:: WasmVM::Opcode::Array_init_elem = 0xFB13
