Instructions
============

.. toctree::
   :maxdepth: 1
   
   Numeric instructions <instr/numeric>
   Reference instructions <instr/reference>
   Parametric instructions <instr/parametric>

Types & Templates
-----------------

.. cpp:type:: WasmVM::Opcode::opcode_t = int16_t

.. cpp:enum:: WasmVM::Opcode::Opcode : opcode_t

.. cpp:struct:: WasmVM::Instr::Base

    .. cpp:function:: Base(Opcode::opcode_t opcode)

    .. cpp:member:: Opcode::opcode_t opcode

.. cpp:struct:: template<Opcode::Opcode OP> WasmVM::Instr::Atomic : public Base

.. cpp:struct:: template<Opcode::Opcode OP> WasmVM::Instr::OneIndex : public Base

    .. cpp:function:: OneIndex(index_t index)

    .. cpp:member:: index_t index

.. cpp:struct:: template<Opcode::Opcode OP> WasmVM::Instr::BlockInstr : public Base

    .. cpp:function:: BlockInstr(std::optional<index_t> type)

    .. cpp:member:: std::optional<index_t> type

   