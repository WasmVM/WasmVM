Structure
=========

Data structures of WasmVM objects for general use.

.. cpp:struct:: WasmVM::WasmModule
    
    .. cpp:member:: std::vector<FuncType> types

    .. cpp:member:: std::vector<WasmImport> imports

    .. cpp:member:: std::vector<WasmFunc> funcs

    .. cpp:member:: std::vector<TableType> tables

    .. cpp:member:: std::vector<MemType> mems

    .. cpp:member:: std::vector<WasmGlobal> globals

    .. cpp:member:: std::vector<WasmExport> exports

    .. cpp:member:: std::optional<index_t> start

    .. cpp:member:: std::vector<WasmElem> elems

    .. cpp:member:: std::vector<WasmData> datas

.. cpp:struct:: WasmVM::WasmFunc

    Function in WebAssembly

    .. cpp:member:: index_t typeidx

        Function type index

    .. cpp:member:: std::vector<ValueType> locals

        Local variables

    .. cpp:member:: std::vector<WasmInstr> body

        Function body

.. cpp:struct:: WasmVM::WasmGlobal

    .. cpp:member:: GlobalType type

    .. cpp:member:: ConstInstr init

.. cpp:struct:: WasmVM::WasmElem

    .. cpp:member:: RefType type

    .. cpp:member:: std::vector<ConstInstr> elemlist
        
    .. cpp:member:: ElemMode mode

    .. cpp:struct:: ElemMode

        .. cpp:enum:: Mode

            .. cpp:enumerator:: passive

            .. cpp:enumerator:: active

            .. cpp:enumerator:: declarative

        .. cpp:member:: Mode type

        .. cpp:member:: std::optional<index_t> tableidx

        .. cpp:member:: std::optional<ConstInstr> offset

.. cpp:struct:: WasmVM::WasmData

    .. cpp:member:: std::vector<byte_t> init
        
    .. cpp:member:: DataMode mode

    .. cpp:struct:: DataMode

        .. cpp:enum:: Mode

            .. cpp:enumerator:: passive

            .. cpp:enumerator:: active

        .. cpp:member:: Mode type

        .. cpp:member:: std::optional<index_t> memidx

        .. cpp:member:: std::optional<ConstInstr> offset

.. cpp:struct:: WasmVM::WasmExport

    .. cpp:enum:: DescType

            .. cpp:enumerator:: func = 0x00

            .. cpp:enumerator:: table = 0x01

            .. cpp:enumerator:: mem = 0x02

            .. cpp:enumerator:: global = 0x03
    
    .. cpp:member:: std::string name

    .. cpp:member:: DescType desc

    .. cpp:member:: index_t index

.. cpp:struct:: WasmVM::WasmImport
    
    .. cpp:member:: std::string module

    .. cpp:member:: std::string name

    .. cpp:member:: std::variant<index_t, TableType, MemType, GlobalType> desc
        
.. cpp:type:: WasmVM::ConstInstr = std::variant<Instr::Ref_null, Instr::Ref_func, \
    Instr::Global_get, Instr::I32_const, Instr::I64_const, \
    Instr::F32_const, Instr::F64_const>

.. cpp:type:: WasmVM::WasmInstr = std::variant< \
    Instr::Unreachable, \
    Instr::Nop, \
    Instr::Block, \
    Instr::Loop, \
    Instr::If, \
    Instr::Else, \
    Instr::End, \
    Instr::Br, \
    Instr::Br_if, \
    Instr::Br_table, \
    Instr::Return, \
    Instr::Call, \
    Instr::Call_indirect, \
    Instr::Ref_null, \
    Instr::Ref_is_null, \
    Instr::Ref_func, \
    Instr::Drop, \
    Instr::Select, \
    Instr::Local_get, \
    Instr::Local_set, \
    Instr::Local_tee, \
    Instr::Global_get, \
    Instr::Global_set, \
    Instr::Table_get, \
    Instr::Table_set, \
    Instr::Table_size, \
    Instr::Table_grow, \
    Instr::Table_fill, \
    Instr::Table_copy, \
    Instr::Table_init, \
    Instr::Elem_drop, \
    Instr::I32_load, \
    Instr::I64_load, \
    Instr::F32_load, \
    Instr::F64_load, \
    Instr::I32_load8_s, \
    Instr::I32_load8_u, \
    Instr::I32_load16_s, \
    Instr::I32_load16_u, \
    Instr::I64_load8_s, \
    Instr::I64_load8_u, \
    Instr::I64_load16_s, \
    Instr::I64_load16_u, \
    Instr::I64_load32_s, \
    Instr::I64_load32_u, \
    Instr::I32_store, \
    Instr::I64_store, \
    Instr::F32_store, \
    Instr::F64_store, \
    Instr::I32_store8, \
    Instr::I32_store16, \
    Instr::I64_store8, \
    Instr::I64_store16, \
    Instr::I64_store32, \
    Instr::Memory_size, \
    Instr::Memory_grow, \
    Instr::Memory_fill, \
    Instr::Memory_init, \
    Instr::Data_drop, \
    Instr::Memory_copy, \
    Instr::I32_const, \
    Instr::I64_const, \
    Instr::F32_const, \
    Instr::F64_const, \
    Instr::I32_eqz, \
    Instr::I32_eq, \
    Instr::I32_ne, \
    Instr::I32_lt_s, \
    Instr::I32_lt_u, \
    Instr::I32_gt_s, \
    Instr::I32_gt_u, \
    Instr::I32_le_s, \
    Instr::I32_le_u, \
    Instr::I32_ge_s, \
    Instr::I32_ge_u, \
    Instr::I64_eqz, \
    Instr::I64_eq, \
    Instr::I64_ne, \
    Instr::I64_lt_s, \
    Instr::I64_lt_u, \
    Instr::I64_gt_s, \
    Instr::I64_gt_u, \
    Instr::I64_le_s, \
    Instr::I64_le_u, \
    Instr::I64_ge_s, \
    Instr::I64_ge_u, \
    Instr::F32_eq, \
    Instr::F32_ne, \
    Instr::F32_lt, \
    Instr::F32_gt, \
    Instr::F32_le, \
    Instr::F32_ge, \
    Instr::F64_eq, \
    Instr::F64_ne, \
    Instr::F64_lt, \
    Instr::F64_gt, \
    Instr::F64_le, \
    Instr::F64_ge, \
    Instr::I32_clz, \
    Instr::I32_ctz, \
    Instr::I32_popcnt, \
    Instr::I32_add, \
    Instr::I32_sub, \
    Instr::I32_mul, \
    Instr::I32_div_s, \
    Instr::I32_div_u, \
    Instr::I32_rem_s, \
    Instr::I32_rem_u, \
    Instr::I32_and, \
    Instr::I32_or, \
    Instr::I32_xor, \
    Instr::I32_shl, \
    Instr::I32_shr_s, \
    Instr::I32_shr_u, \
    Instr::I32_rotl, \
    Instr::I32_rotr, \
    Instr::I64_clz, \
    Instr::I64_ctz, \
    Instr::I64_popcnt, \
    Instr::I64_add, \
    Instr::I64_sub, \
    Instr::I64_mul, \
    Instr::I64_div_s, \
    Instr::I64_div_u, \
    Instr::I64_rem_s, \
    Instr::I64_rem_u, \
    Instr::I64_and, \
    Instr::I64_or, \
    Instr::I64_xor, \
    Instr::I64_shl, \
    Instr::I64_shr_s, \
    Instr::I64_shr_u, \
    Instr::I64_rotl, \
    Instr::I64_rotr, \
    Instr::F32_abs, \
    Instr::F32_neg, \
    Instr::F32_ceil, \
    Instr::F32_floor, \
    Instr::F32_trunc, \
    Instr::F32_nearest, \
    Instr::F32_sqrt, \
    Instr::F32_add, \
    Instr::F32_sub, \
    Instr::F32_mul, \
    Instr::F32_div, \
    Instr::F32_min, \
    Instr::F32_max, \
    Instr::F32_copysign, \
    Instr::F64_abs, \
    Instr::F64_neg, \
    Instr::F64_ceil, \
    Instr::F64_floor, \
    Instr::F64_trunc, \
    Instr::F64_nearest, \
    Instr::F64_sqrt, \
    Instr::F64_add, \
    Instr::F64_sub, \
    Instr::F64_mul, \
    Instr::F64_div, \
    Instr::F64_min, \
    Instr::F64_max, \
    Instr::F64_copysign, \
    Instr::I32_wrap_i64, \
    Instr::I32_trunc_s_f32, \
    Instr::I32_trunc_u_f32, \
    Instr::I32_trunc_s_f64, \
    Instr::I32_trunc_u_f64, \
    Instr::I64_extend_s_i32, \
    Instr::I64_extend_u_i32, \
    Instr::I64_trunc_s_f32, \
    Instr::I64_trunc_u_f32, \
    Instr::I64_trunc_s_f64, \
    Instr::I64_trunc_u_f64, \
    Instr::F32_convert_s_i32, \
    Instr::F32_convert_u_i32, \
    Instr::F32_convert_s_i64, \
    Instr::F32_convert_u_i64, \
    Instr::F32_demote_f64, \
    Instr::F64_convert_s_i32, \
    Instr::F64_convert_u_i32, \
    Instr::F64_convert_s_i64, \
    Instr::F64_convert_u_i64, \
    Instr::F64_promote_f32, \
    Instr::I32_reinterpret_f32, \
    Instr::I64_reinterpret_f64, \
    Instr::F32_reinterpret_i32, \
    Instr::F64_reinterpret_i64, \
    Instr::I32_extend8_s, \
    Instr::I32_extend16_s, \
    Instr::I64_extend8_s, \
    Instr::I64_extend16_s, \
    Instr::I64_extend32_s, \
    Instr::I32_trunc_sat_f32_s, \
    Instr::I32_trunc_sat_f32_u, \
    Instr::I32_trunc_sat_f64_s, \
    Instr::I32_trunc_sat_f64_u, \
    Instr::I64_trunc_sat_f32_s, \
    Instr::I64_trunc_sat_f32_u, \
    Instr::I64_trunc_sat_f64_s, \
    Instr::I64_trunc_sat_f64_u>