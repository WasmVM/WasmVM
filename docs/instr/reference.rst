Reference instructions
======================

Basic reference instructions
----------------------------

.. cpp:struct:: WasmVM::Instr::Ref_null : public Base

    .. cpp:function:: Ref_null(RefType heaptype)

    .. cpp:member:: RefType heaptype = RefType::funcref

.. cpp:enumerator:: WasmVM::Opcode::Ref_null = 0xD0

.. cpp:type:: WasmVM::Instr::Ref_is_null = WasmVM::Instr::Atomic<WasmVM::Opcode::Ref_is_null>
.. cpp:enumerator:: WasmVM::Opcode::Ref_is_null = 0xD1

.. cpp:type:: WasmVM::Instr::Ref_func = WasmVM::Instr::OneIndex<WasmVM::Opcode::Ref_func>
.. cpp:enumerator:: WasmVM::Opcode::Ref_func = 0xD2

.. cpp:type:: WasmVM::Instr::Ref_eq = WasmVM::Instr::Atomic<WasmVM::Opcode::Ref_eq>
.. cpp:enumerator:: WasmVM::Opcode::Ref_eq = 0xD3

Typed function references
-------------------------

.. cpp:type:: WasmVM::Instr::Ref_as_non_null = WasmVM::Instr::Atomic<WasmVM::Opcode::Ref_as_non_null>
.. cpp:enumerator:: WasmVM::Opcode::Ref_as_non_null = 0xD4

.. cpp:type:: WasmVM::Instr::Br_on_null = WasmVM::Instr::OneIndex<WasmVM::Opcode::Br_on_null>
.. cpp:enumerator:: WasmVM::Opcode::Br_on_null = 0xD5

.. cpp:type:: WasmVM::Instr::Br_on_non_null = WasmVM::Instr::OneIndex<WasmVM::Opcode::Br_on_non_null>
.. cpp:enumerator:: WasmVM::Opcode::Br_on_non_null = 0xD6

Type tests and casts
--------------------

The ``heaptype`` field is a signed 32-bit value: negative values denote
abstract heap types (e.g. ``-16`` = ``func``, ``-17`` = ``extern``,
``-18`` = ``any``, ``-19`` = ``eq``, ``-20`` = ``i31``, ``-21`` = ``struct``,
``-22`` = ``array``, ``-23`` = ``none``, ``-24`` = ``noextern``,
``-25`` = ``nofunc``); non-negative values are type indices.

.. cpp:struct:: WasmVM::Instr::Ref_test : public Base

    .. cpp:function:: Ref_test(int32_t heaptype)
    .. cpp:member:: int32_t heaptype

.. cpp:enumerator:: WasmVM::Opcode::Ref_test = 0xFB14

.. cpp:struct:: WasmVM::Instr::Ref_test_null : public Base

    .. cpp:function:: Ref_test_null(int32_t heaptype)
    .. cpp:member:: int32_t heaptype

.. cpp:enumerator:: WasmVM::Opcode::Ref_test_null = 0xFB15

.. cpp:struct:: WasmVM::Instr::Ref_cast : public Base

    .. cpp:function:: Ref_cast(int32_t heaptype)
    .. cpp:member:: int32_t heaptype

.. cpp:enumerator:: WasmVM::Opcode::Ref_cast = 0xFB16

.. cpp:struct:: WasmVM::Instr::Ref_cast_null : public Base

    .. cpp:function:: Ref_cast_null(int32_t heaptype)
    .. cpp:member:: int32_t heaptype

.. cpp:enumerator:: WasmVM::Opcode::Ref_cast_null = 0xFB17

.. cpp:struct:: WasmVM::Instr::Br_on_cast : public Base

    .. cpp:function:: Br_on_cast(index_t labelidx, bool src_nullable, bool dst_nullable, int32_t src_heaptype, int32_t dst_heaptype)
    .. cpp:member:: index_t labelidx
    .. cpp:member:: bool src_nullable
    .. cpp:member:: bool dst_nullable
    .. cpp:member:: int32_t src_heaptype
    .. cpp:member:: int32_t dst_heaptype

.. cpp:enumerator:: WasmVM::Opcode::Br_on_cast = 0xFB18

.. cpp:struct:: WasmVM::Instr::Br_on_cast_fail : public Base

    .. cpp:function:: Br_on_cast_fail(index_t labelidx, bool src_nullable, bool dst_nullable, int32_t src_heaptype, int32_t dst_heaptype)
    .. cpp:member:: index_t labelidx
    .. cpp:member:: bool src_nullable
    .. cpp:member:: bool dst_nullable
    .. cpp:member:: int32_t src_heaptype
    .. cpp:member:: int32_t dst_heaptype

.. cpp:enumerator:: WasmVM::Opcode::Br_on_cast_fail = 0xFB19

i31 scalar references
---------------------

.. cpp:type:: WasmVM::Instr::Ref_i31 = WasmVM::Instr::Atomic<WasmVM::Opcode::Ref_i31>
.. cpp:enumerator:: WasmVM::Opcode::Ref_i31 = 0xFB1C

.. cpp:type:: WasmVM::Instr::I31_get_s = WasmVM::Instr::Atomic<WasmVM::Opcode::I31_get_s>
.. cpp:enumerator:: WasmVM::Opcode::I31_get_s = 0xFB1D

.. cpp:type:: WasmVM::Instr::I31_get_u = WasmVM::Instr::Atomic<WasmVM::Opcode::I31_get_u>
.. cpp:enumerator:: WasmVM::Opcode::I31_get_u = 0xFB1E

GC type conversions
-------------------

.. cpp:type:: WasmVM::Instr::Any_convert_extern = WasmVM::Instr::Atomic<WasmVM::Opcode::Any_convert_extern>
.. cpp:enumerator:: WasmVM::Opcode::Any_convert_extern = 0xFB1A

.. cpp:type:: WasmVM::Instr::Extern_convert_any = WasmVM::Instr::Atomic<WasmVM::Opcode::Extern_convert_any>
.. cpp:enumerator:: WasmVM::Opcode::Extern_convert_any = 0xFB1B
