// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include "RunVisitor.hpp"

using namespace WasmVM;

// Dispatch macros — reconstruct Instr::* temporaries and call the appropriate operator().
#define ATOM(OP, TYPE) \
    case Opcode::OP: { Instr::TYPE t; operator()(t); break; }
#define ONE(OP, TYPE) \
    case Opcode::OP: { auto& _o = std::get<WasmInstr::OneIdx>(instr.imm); Instr::TYPE t{_o.index}; operator()(t); break; }
#define TWO(OP, TYPE, A, B) \
    case Opcode::OP: { auto& _tw = std::get<WasmInstr::TwoIdx>(instr.imm); Instr::TYPE t{_tw.A, _tw.B}; operator()(t); break; }
#define BLK(OP, TYPE) \
    case Opcode::OP: { auto& _b = std::get<WasmInstr::BlockType>(instr.imm); Instr::TYPE t{_b.type}; operator()(t); break; }
#define MEM(OP, TYPE) \
    case Opcode::OP: { auto& _m = std::get<WasmInstr::MemArg>(instr.imm); Instr::TYPE t{_m.memidx, _m.offset, _m.align}; operator()(t); break; }

void RunVisitor::run(WasmInstr& instr) {
    switch(instr.opcode) {
    // --- Control: no-imm ---
    ATOM(Unreachable, Unreachable)
    ATOM(Nop, Nop)
    ATOM(Else, Else)
    ATOM(End, End)
    ATOM(Return, Return)
    ATOM(Throw_ref, Throw_ref)
    // --- Control: block type ---
    BLK(Block, Block)
    BLK(Loop, Loop)
    BLK(If, If)
    // --- Control: one-index ---
    ONE(Br, Br)
    ONE(Br_if, Br_if)
    ONE(Throw, Throw)
    ONE(Call, Call)
    ONE(Return_call, Return_call)
    ONE(Return_call_ref, Return_call_ref)
    ONE(Call_ref, Call_ref)
    // --- Control: two-index ---
    TWO(Call_indirect, Call_indirect, a, b)
    TWO(Return_call_indirect, Return_call_indirect, a, b)
    // --- Control: Br_table ---
    case Opcode::Br_table: {
        auto& bt = std::get<WasmInstr::BrTab>(instr.imm);
        Instr::Br_table t;
        t.indices = bt.indices;
        operator()(t);
        break;
    }
    // --- Control: Try_table ---
    case Opcode::Try_table: {
        auto& tt = std::get<WasmInstr::TryTab>(instr.imm);
        Instr::Try_table t;
        t.type = tt.type;
        t.catches = tt.catches;
        operator()(t);
        break;
    }

    // --- Reference: no-imm ---
    ATOM(Ref_is_null, Ref_is_null)
    ATOM(Ref_eq, Ref_eq)
    ATOM(Ref_as_non_null, Ref_as_non_null)
    ATOM(Ref_i31, Ref_i31)
    ATOM(I31_get_s, I31_get_s)
    ATOM(I31_get_u, I31_get_u)
    ATOM(Any_convert_extern, Any_convert_extern)
    ATOM(Extern_convert_any, Extern_convert_any)
    // --- Reference: one-index ---
    ONE(Ref_func, Ref_func)
    ONE(Br_on_null, Br_on_null)
    ONE(Br_on_non_null, Br_on_non_null)
    // --- Reference: heap-ref ---
    case Opcode::Ref_null: {
        auto& h = std::get<WasmInstr::HeapRef>(instr.imm);
        Instr::Ref_null t{h.heaptype};
        operator()(t);
        break;
    }
    // --- Reference: heap-i32 ---
    case Opcode::Ref_test: {
        auto& h = std::get<WasmInstr::HeapI32>(instr.imm);
        Instr::Ref_test t{h.heaptype};
        operator()(t);
        break;
    }
    case Opcode::Ref_test_null: {
        auto& h = std::get<WasmInstr::HeapI32>(instr.imm);
        Instr::Ref_test_null t{h.heaptype};
        operator()(t);
        break;
    }
    case Opcode::Ref_cast: {
        auto& h = std::get<WasmInstr::HeapI32>(instr.imm);
        Instr::Ref_cast t{h.heaptype};
        operator()(t);
        break;
    }
    case Opcode::Ref_cast_null: {
        auto& h = std::get<WasmInstr::HeapI32>(instr.imm);
        Instr::Ref_cast_null t{h.heaptype};
        operator()(t);
        break;
    }
    // --- Reference: cast-op ---
    case Opcode::Br_on_cast: {
        auto& c = std::get<WasmInstr::CastOp>(instr.imm);
        Instr::Br_on_cast t{c.label, c.src_n, c.dst_n, c.src_ht, c.dst_ht};
        operator()(t);
        break;
    }
    case Opcode::Br_on_cast_fail: {
        auto& c = std::get<WasmInstr::CastOp>(instr.imm);
        Instr::Br_on_cast_fail t{c.label, c.src_n, c.dst_n, c.src_ht, c.dst_ht};
        operator()(t);
        break;
    }

    // --- GC Struct ---
    ONE(Struct_new, Struct_new)
    ONE(Struct_new_default, Struct_new_default)
    TWO(Struct_get, Struct_get, a, b)
    TWO(Struct_get_s, Struct_get_s, a, b)
    TWO(Struct_get_u, Struct_get_u, a, b)
    TWO(Struct_set, Struct_set, a, b)

    // --- GC Array ---
    ONE(Array_new, Array_new)
    ONE(Array_new_default, Array_new_default)
    TWO(Array_new_fixed, Array_new_fixed, a, b)
    TWO(Array_new_data, Array_new_data, a, b)
    TWO(Array_new_elem, Array_new_elem, a, b)
    ONE(Array_get, Array_get)
    ONE(Array_get_s, Array_get_s)
    ONE(Array_get_u, Array_get_u)
    ONE(Array_set, Array_set)
    ATOM(Array_len, Array_len)
    ONE(Array_fill, Array_fill)
    TWO(Array_copy, Array_copy, a, b)
    TWO(Array_init_data, Array_init_data, a, b)
    TWO(Array_init_elem, Array_init_elem, a, b)

    // --- Parametric ---
    ATOM(Drop, Drop)
    case Opcode::Select: {
        auto& sv = std::get<WasmInstr::SelectV>(instr.imm);
        Instr::Select t;
        t.valtypes = sv.valtypes;
        operator()(t);
        break;
    }

    // --- Variable ---
    ONE(Local_get, Local_get)
    ONE(Local_set, Local_set)
    ONE(Local_tee, Local_tee)
    ONE(Global_get, Global_get)
    ONE(Global_set, Global_set)

    // --- Table ---
    ONE(Table_get, Table_get)
    ONE(Table_set, Table_set)
    ONE(Table_size, Table_size)
    ONE(Table_grow, Table_grow)
    ONE(Table_fill, Table_fill)
    TWO(Table_copy, Table_copy, a, b)
    TWO(Table_init, Table_init, a, b)
    ONE(Elem_drop, Elem_drop)

    // --- Memory: load/store (MemArg) ---
    MEM(I32_load, I32_load)
    MEM(I64_load, I64_load)
    MEM(F32_load, F32_load)
    MEM(F64_load, F64_load)
    MEM(I32_load8_s, I32_load8_s)
    MEM(I32_load8_u, I32_load8_u)
    MEM(I32_load16_s, I32_load16_s)
    MEM(I32_load16_u, I32_load16_u)
    MEM(I64_load8_s, I64_load8_s)
    MEM(I64_load8_u, I64_load8_u)
    MEM(I64_load16_s, I64_load16_s)
    MEM(I64_load16_u, I64_load16_u)
    MEM(I64_load32_s, I64_load32_s)
    MEM(I64_load32_u, I64_load32_u)
    MEM(I32_store, I32_store)
    MEM(I64_store, I64_store)
    MEM(F32_store, F32_store)
    MEM(F64_store, F64_store)
    MEM(I32_store8, I32_store8)
    MEM(I32_store16, I32_store16)
    MEM(I64_store8, I64_store8)
    MEM(I64_store16, I64_store16)
    MEM(I64_store32, I64_store32)
    // --- Memory: one-index ---
    ONE(Memory_size, Memory_size)
    ONE(Memory_grow, Memory_grow)
    ONE(Memory_fill, Memory_fill)
    ONE(Data_drop, Data_drop)
    // --- Memory: two-index ---
    TWO(Memory_init, Memory_init, a, b)
    TWO(Memory_copy, Memory_copy, a, b)

    // --- Numeric: consts ---
    case Opcode::I32_const: { auto& c = std::get<WasmInstr::ConstI32>(instr.imm); Instr::I32_const t{c.value}; operator()(t); break; }
    case Opcode::I64_const: { auto& c = std::get<WasmInstr::ConstI64>(instr.imm); Instr::I64_const t{c.value}; operator()(t); break; }
    case Opcode::F32_const: { auto& c = std::get<WasmInstr::ConstF32>(instr.imm); Instr::F32_const t{c.value}; operator()(t); break; }
    case Opcode::F64_const: { auto& c = std::get<WasmInstr::ConstF64>(instr.imm); Instr::F64_const t{c.value}; operator()(t); break; }

    // --- i32 atomic ---
    ATOM(I32_eqz, I32_eqz)     ATOM(I32_eq, I32_eq)       ATOM(I32_ne, I32_ne)
    ATOM(I32_lt_s, I32_lt_s)   ATOM(I32_lt_u, I32_lt_u)
    ATOM(I32_gt_s, I32_gt_s)   ATOM(I32_gt_u, I32_gt_u)
    ATOM(I32_le_s, I32_le_s)   ATOM(I32_le_u, I32_le_u)
    ATOM(I32_ge_s, I32_ge_s)   ATOM(I32_ge_u, I32_ge_u)
    ATOM(I32_clz, I32_clz)     ATOM(I32_ctz, I32_ctz)     ATOM(I32_popcnt, I32_popcnt)
    ATOM(I32_add, I32_add)     ATOM(I32_sub, I32_sub)     ATOM(I32_mul, I32_mul)
    ATOM(I32_div_s, I32_div_s) ATOM(I32_div_u, I32_div_u)
    ATOM(I32_rem_s, I32_rem_s) ATOM(I32_rem_u, I32_rem_u)
    ATOM(I32_and, I32_and)     ATOM(I32_or, I32_or)       ATOM(I32_xor, I32_xor)
    ATOM(I32_shl, I32_shl)     ATOM(I32_shr_s, I32_shr_s) ATOM(I32_shr_u, I32_shr_u)
    ATOM(I32_rotl, I32_rotl)   ATOM(I32_rotr, I32_rotr)
    ATOM(I32_wrap_i64, I32_wrap_i64)
    ATOM(I32_trunc_f32_s, I32_trunc_f32_s) ATOM(I32_trunc_f32_u, I32_trunc_f32_u)
    ATOM(I32_trunc_f64_s, I32_trunc_f64_s) ATOM(I32_trunc_f64_u, I32_trunc_f64_u)
    ATOM(I32_reinterpret_f32, I32_reinterpret_f32)
    ATOM(I32_extend8_s, I32_extend8_s)   ATOM(I32_extend16_s, I32_extend16_s)
    ATOM(I32_trunc_sat_f32_s, I32_trunc_sat_f32_s) ATOM(I32_trunc_sat_f32_u, I32_trunc_sat_f32_u)
    ATOM(I32_trunc_sat_f64_s, I32_trunc_sat_f64_s) ATOM(I32_trunc_sat_f64_u, I32_trunc_sat_f64_u)

    // --- i64 atomic ---
    ATOM(I64_eqz, I64_eqz)     ATOM(I64_eq, I64_eq)       ATOM(I64_ne, I64_ne)
    ATOM(I64_lt_s, I64_lt_s)   ATOM(I64_lt_u, I64_lt_u)
    ATOM(I64_gt_s, I64_gt_s)   ATOM(I64_gt_u, I64_gt_u)
    ATOM(I64_le_s, I64_le_s)   ATOM(I64_le_u, I64_le_u)
    ATOM(I64_ge_s, I64_ge_s)   ATOM(I64_ge_u, I64_ge_u)
    ATOM(I64_clz, I64_clz)     ATOM(I64_ctz, I64_ctz)     ATOM(I64_popcnt, I64_popcnt)
    ATOM(I64_add, I64_add)     ATOM(I64_sub, I64_sub)     ATOM(I64_mul, I64_mul)
    ATOM(I64_div_s, I64_div_s) ATOM(I64_div_u, I64_div_u)
    ATOM(I64_rem_s, I64_rem_s) ATOM(I64_rem_u, I64_rem_u)
    ATOM(I64_and, I64_and)     ATOM(I64_or, I64_or)       ATOM(I64_xor, I64_xor)
    ATOM(I64_shl, I64_shl)     ATOM(I64_shr_s, I64_shr_s) ATOM(I64_shr_u, I64_shr_u)
    ATOM(I64_rotl, I64_rotl)   ATOM(I64_rotr, I64_rotr)
    ATOM(I64_extend_i32_s, I64_extend_i32_s) ATOM(I64_extend_i32_u, I64_extend_i32_u)
    ATOM(I64_trunc_f32_s, I64_trunc_f32_s)   ATOM(I64_trunc_f32_u, I64_trunc_f32_u)
    ATOM(I64_trunc_f64_s, I64_trunc_f64_s)   ATOM(I64_trunc_f64_u, I64_trunc_f64_u)
    ATOM(I64_reinterpret_f64, I64_reinterpret_f64)
    ATOM(I64_extend8_s, I64_extend8_s) ATOM(I64_extend16_s, I64_extend16_s) ATOM(I64_extend32_s, I64_extend32_s)
    ATOM(I64_trunc_sat_f32_s, I64_trunc_sat_f32_s) ATOM(I64_trunc_sat_f32_u, I64_trunc_sat_f32_u)
    ATOM(I64_trunc_sat_f64_s, I64_trunc_sat_f64_s) ATOM(I64_trunc_sat_f64_u, I64_trunc_sat_f64_u)

    // --- f32 atomic ---
    ATOM(F32_eq, F32_eq)   ATOM(F32_ne, F32_ne)   ATOM(F32_lt, F32_lt)
    ATOM(F32_gt, F32_gt)   ATOM(F32_le, F32_le)   ATOM(F32_ge, F32_ge)
    ATOM(F32_abs, F32_abs) ATOM(F32_neg, F32_neg) ATOM(F32_ceil, F32_ceil)
    ATOM(F32_floor, F32_floor) ATOM(F32_trunc, F32_trunc) ATOM(F32_nearest, F32_nearest)
    ATOM(F32_sqrt, F32_sqrt)
    ATOM(F32_add, F32_add) ATOM(F32_sub, F32_sub) ATOM(F32_mul, F32_mul) ATOM(F32_div, F32_div)
    ATOM(F32_min, F32_min) ATOM(F32_max, F32_max) ATOM(F32_copysign, F32_copysign)
    ATOM(F32_reinterpret_i32, F32_reinterpret_i32)
    ATOM(F32_convert_i32_s, F32_convert_i32_s) ATOM(F32_convert_i32_u, F32_convert_i32_u)
    ATOM(F32_convert_i64_s, F32_convert_i64_s) ATOM(F32_convert_i64_u, F32_convert_i64_u)
    ATOM(F32_demote_f64, F32_demote_f64)

    // --- f64 atomic ---
    ATOM(F64_eq, F64_eq)   ATOM(F64_ne, F64_ne)   ATOM(F64_lt, F64_lt)
    ATOM(F64_gt, F64_gt)   ATOM(F64_le, F64_le)   ATOM(F64_ge, F64_ge)
    ATOM(F64_abs, F64_abs) ATOM(F64_neg, F64_neg) ATOM(F64_ceil, F64_ceil)
    ATOM(F64_floor, F64_floor) ATOM(F64_trunc, F64_trunc) ATOM(F64_nearest, F64_nearest)
    ATOM(F64_sqrt, F64_sqrt)
    ATOM(F64_add, F64_add) ATOM(F64_sub, F64_sub) ATOM(F64_mul, F64_mul) ATOM(F64_div, F64_div)
    ATOM(F64_min, F64_min) ATOM(F64_max, F64_max) ATOM(F64_copysign, F64_copysign)
    ATOM(F64_reinterpret_i64, F64_reinterpret_i64)
    ATOM(F64_convert_i32_s, F64_convert_i32_s) ATOM(F64_convert_i32_u, F64_convert_i32_u)
    ATOM(F64_convert_i64_s, F64_convert_i64_s) ATOM(F64_convert_i64_u, F64_convert_i64_u)
    ATOM(F64_promote_f32, F64_promote_f32)

    default: break;
    }
}

#undef ATOM
#undef ONE
#undef TWO
#undef BLK
#undef MEM
