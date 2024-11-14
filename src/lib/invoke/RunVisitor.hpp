#ifndef WASMVM_PP_INVOKE_RunVisitor_DEF
#define WASMVM_PP_INVOKE_RunVisitor_DEF

#include <instances/Stack.hpp>
#include <optional>
#include <vector>
#include <exception.hpp>

using namespace WasmVM;

struct RunVisitor {
    RunVisitor(Stack& stack) : stack(stack){};
    Stack& stack;

    // Control
    void operator()(Instr::Unreachable&);
    void operator()(Instr::Nop&);
    void operator()(Instr::Block&);
    void operator()(Instr::Loop&);
    void operator()(Instr::If&);
    void operator()(Instr::Else&);
    void operator()(Instr::End&);
    void operator()(Instr::Br&);
    void operator()(Instr::Br_if&);
    void operator()(Instr::Br_table&);
    void operator()(Instr::Return&);
    void operator()(Instr::Call&);
    void operator()(Instr::Call_indirect&);
    // Reference
    void operator()(Instr::Ref_null&);
    void operator()(Instr::Ref_is_null&);
    void operator()(Instr::Ref_func&);
    // Parametric
    void operator()(Instr::Drop&);
    void operator()(Instr::Select&);
    // Variable
    void operator()(Instr::Local_get&);
    void operator()(Instr::Local_set&);
    void operator()(Instr::Local_tee&);
    void operator()(Instr::Global_get&);
    void operator()(Instr::Global_set&);
    // Table
    void operator()(Instr::Table_get&);
    void operator()(Instr::Table_set&);
    void operator()(Instr::Table_size&);
    void operator()(Instr::Table_grow&);
    void operator()(Instr::Table_fill&);
    void operator()(Instr::Table_copy&);
    void operator()(Instr::Table_init&);
    void operator()(Instr::Elem_drop&);
    // Memory
    void operator()(Instr::I32_load&);
    void operator()(Instr::I64_load&);
    void operator()(Instr::F32_load&);
    void operator()(Instr::F64_load&);
    void operator()(Instr::I32_load8_s&);
    void operator()(Instr::I32_load8_u&);
    void operator()(Instr::I32_load16_s&);
    void operator()(Instr::I32_load16_u&);
    void operator()(Instr::I64_load8_s&);
    void operator()(Instr::I64_load8_u&);
    void operator()(Instr::I64_load16_s&);
    void operator()(Instr::I64_load16_u&);
    void operator()(Instr::I64_load32_s&);
    void operator()(Instr::I64_load32_u&);
    void operator()(Instr::I32_store&);
    void operator()(Instr::I64_store&);
    void operator()(Instr::F32_store&);
    void operator()(Instr::F64_store&);
    void operator()(Instr::I32_store8&);
    void operator()(Instr::I32_store16&);
    void operator()(Instr::I64_store8&);
    void operator()(Instr::I64_store16&);
    void operator()(Instr::I64_store32&);
    void operator()(Instr::Memory_size&);
    void operator()(Instr::Memory_grow&);
    void operator()(Instr::Memory_fill&);
    void operator()(Instr::Memory_init&);
    void operator()(Instr::Data_drop&);
    void operator()(Instr::Memory_copy&);
    // i32
    void operator()(Instr::I32_const&);
    void operator()(Instr::I32_eqz&);
    void operator()(Instr::I32_eq&);
    void operator()(Instr::I32_ne&);
    void operator()(Instr::I32_lt_s&);
    void operator()(Instr::I32_lt_u&);
    void operator()(Instr::I32_gt_s&);
    void operator()(Instr::I32_gt_u&);
    void operator()(Instr::I32_le_s&);
    void operator()(Instr::I32_le_u&);
    void operator()(Instr::I32_ge_s&);
    void operator()(Instr::I32_ge_u&);
    void operator()(Instr::I32_wrap_i64&);
    void operator()(Instr::I32_trunc_f32_s&);
    void operator()(Instr::I32_trunc_f32_u&);
    void operator()(Instr::I32_trunc_f64_s&);
    void operator()(Instr::I32_trunc_f64_u&);
    void operator()(Instr::I32_clz&);
    void operator()(Instr::I32_ctz&);
    void operator()(Instr::I32_popcnt&);
    void operator()(Instr::I32_add&);
    void operator()(Instr::I32_sub&);
    void operator()(Instr::I32_mul&);
    void operator()(Instr::I32_div_s&);
    void operator()(Instr::I32_div_u&);
    void operator()(Instr::I32_rem_s&);
    void operator()(Instr::I32_rem_u&);
    void operator()(Instr::I32_and&);
    void operator()(Instr::I32_or&);
    void operator()(Instr::I32_xor&);
    void operator()(Instr::I32_shl&);
    void operator()(Instr::I32_shr_s&);
    void operator()(Instr::I32_shr_u&);
    void operator()(Instr::I32_rotl&);
    void operator()(Instr::I32_rotr&);
    void operator()(Instr::I32_reinterpret_f32&);
    void operator()(Instr::I32_extend8_s&);
    void operator()(Instr::I32_extend16_s&);
    void operator()(Instr::I32_trunc_sat_f32_s&);
    void operator()(Instr::I32_trunc_sat_f32_u&);
    void operator()(Instr::I32_trunc_sat_f64_s&);
    void operator()(Instr::I32_trunc_sat_f64_u&);
    // i64
    void operator()(Instr::I64_const&);
    void operator()(Instr::I64_eqz&);
    void operator()(Instr::I64_eq&);
    void operator()(Instr::I64_ne&);
    void operator()(Instr::I64_lt_s&);
    void operator()(Instr::I64_lt_u&);
    void operator()(Instr::I64_gt_s&);
    void operator()(Instr::I64_gt_u&);
    void operator()(Instr::I64_le_s&);
    void operator()(Instr::I64_le_u&);
    void operator()(Instr::I64_ge_s&);
    void operator()(Instr::I64_ge_u&);
    void operator()(Instr::I64_clz&);
    void operator()(Instr::I64_ctz&);
    void operator()(Instr::I64_popcnt&);
    void operator()(Instr::I64_add&);
    void operator()(Instr::I64_sub&);
    void operator()(Instr::I64_mul&);
    void operator()(Instr::I64_div_s&);
    void operator()(Instr::I64_div_u&);
    void operator()(Instr::I64_rem_s&);
    void operator()(Instr::I64_rem_u&);
    void operator()(Instr::I64_and&);
    void operator()(Instr::I64_or&);
    void operator()(Instr::I64_xor&);
    void operator()(Instr::I64_shl&);
    void operator()(Instr::I64_shr_s&);
    void operator()(Instr::I64_shr_u&);
    void operator()(Instr::I64_rotl&);
    void operator()(Instr::I64_rotr&);
    void operator()(Instr::I64_extend_i32_s&);
    void operator()(Instr::I64_extend_i32_u&);
    void operator()(Instr::I64_trunc_f32_s&);
    void operator()(Instr::I64_trunc_f32_u&);
    void operator()(Instr::I64_trunc_f64_s&);
    void operator()(Instr::I64_trunc_f64_u&);
    void operator()(Instr::I64_reinterpret_f64&);
    void operator()(Instr::I64_extend8_s&);
    void operator()(Instr::I64_extend16_s&);
    void operator()(Instr::I64_extend32_s&);
    void operator()(Instr::I64_trunc_sat_f32_s&);
    void operator()(Instr::I64_trunc_sat_f32_u&);
    void operator()(Instr::I64_trunc_sat_f64_s&);
    void operator()(Instr::I64_trunc_sat_f64_u&);
    // f32
    void operator()(Instr::F32_const&);
    void operator()(Instr::F32_eq&);
    void operator()(Instr::F32_ne&);
    void operator()(Instr::F32_lt&);
    void operator()(Instr::F32_gt&);
    void operator()(Instr::F32_le&);
    void operator()(Instr::F32_ge&);
    void operator()(Instr::F32_abs&);
    void operator()(Instr::F32_neg&);
    void operator()(Instr::F32_ceil&);
    void operator()(Instr::F32_floor&);
    void operator()(Instr::F32_trunc&);
    void operator()(Instr::F32_nearest&);
    void operator()(Instr::F32_sqrt&);
    void operator()(Instr::F32_add&);
    void operator()(Instr::F32_sub&);
    void operator()(Instr::F32_mul&);
    void operator()(Instr::F32_div&);
    void operator()(Instr::F32_min&);
    void operator()(Instr::F32_max&);
    void operator()(Instr::F32_copysign&);
    void operator()(Instr::F32_reinterpret_i32&);
    void operator()(Instr::F32_convert_i32_s&);
    void operator()(Instr::F32_convert_i32_u&);
    void operator()(Instr::F32_convert_i64_s&);
    void operator()(Instr::F32_convert_i64_u&);
    void operator()(Instr::F32_demote_f64&);
    // f64
    void operator()(Instr::F64_const&);
    void operator()(Instr::F64_eq&);
    void operator()(Instr::F64_ne&);
    void operator()(Instr::F64_lt&);
    void operator()(Instr::F64_gt&);
    void operator()(Instr::F64_le&);
    void operator()(Instr::F64_ge&);
    void operator()(Instr::F64_abs&);
    void operator()(Instr::F64_neg&);
    void operator()(Instr::F64_ceil&);
    void operator()(Instr::F64_floor&);
    void operator()(Instr::F64_trunc&);
    void operator()(Instr::F64_nearest&);
    void operator()(Instr::F64_sqrt&);
    void operator()(Instr::F64_add&);
    void operator()(Instr::F64_sub&);
    void operator()(Instr::F64_mul&);
    void operator()(Instr::F64_div&);
    void operator()(Instr::F64_min&);
    void operator()(Instr::F64_max&);
    void operator()(Instr::F64_copysign&);
    void operator()(Instr::F64_reinterpret_i64&);
    void operator()(Instr::F64_convert_i32_s&);
    void operator()(Instr::F64_convert_i32_u&);
    void operator()(Instr::F64_convert_i64_s&);
    void operator()(Instr::F64_convert_i64_u&);
    void operator()(Instr::F64_promote_f32&);
};

#endif