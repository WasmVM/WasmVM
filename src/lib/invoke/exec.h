#ifndef WASMVM_INVOKE_EXEC_
#define WASMVM_INVOKE_EXEC_

#include <defines.h>
#include <Stack.h>
#include <Store.h>

void exec_unreachable(wasm_stack label);
void exec_nop(wasm_stack label);
void exec_block(wasm_stack* label, wasm_stack frame, wasm_stack* stack);
void exec_loop(wasm_stack* label, wasm_stack frame, wasm_stack* stack);
void exec_if(wasm_stack* label, wasm_stack frame, wasm_stack* stack);
void exec_else(wasm_stack label);
void exec_end(wasm_stack* label, wasm_stack* frame, wasm_stack* stack);
void exec_br(wasm_stack* label, wasm_stack* frame, wasm_stack* stack);
void exec_br_if(wasm_stack* label, wasm_stack* frame, wasm_stack* stack);
void exec_br_table(wasm_stack* label, wasm_stack* frame, wasm_stack* stack);
void exec_return(wasm_stack* label, wasm_stack* frame, wasm_stack* stack);
void exec_call(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store);
void exec_call_indirect(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_drop(wasm_stack label, wasm_stack* stack);
void exec_select(wasm_stack label, wasm_stack* stack);
void exec_select_t(wasm_stack label, wasm_stack* stack);
void exec_local_get(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_local_set(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_local_tee(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_global_get(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_global_set(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_table_get(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_set(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_i32_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_f32_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_f64_load(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_load8_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_load8_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_load16_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_load16_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load8_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load8_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load16_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load16_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load32_s(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_load32_u(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_f32_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_f64_store(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_store8(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i32_store16(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_store8(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_store16(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_i64_store32(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_memory_size(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_memory_grow(wasm_stack label, wasm_stack frame, wasm_stack* stack, wasm_store store);
void exec_const(wasm_stack label, wasm_stack* stack);
void exec_i32_eqz(wasm_stack label, wasm_stack* stack);
void exec_i32_eq(wasm_stack label, wasm_stack* stack);
void exec_i32_ne(wasm_stack label, wasm_stack* stack);
void exec_i32_lt_s(wasm_stack label, wasm_stack* stack);
void exec_i32_lt_u(wasm_stack label, wasm_stack* stack);
void exec_i32_gt_s(wasm_stack label, wasm_stack* stack);
void exec_i32_gt_u(wasm_stack label, wasm_stack* stack);
void exec_i32_le_s(wasm_stack label, wasm_stack* stack);
void exec_i32_le_u(wasm_stack label, wasm_stack* stack);
void exec_i32_ge_s(wasm_stack label, wasm_stack* stack);
void exec_i32_ge_u(wasm_stack label, wasm_stack* stack);
void exec_i64_eqz(wasm_stack label, wasm_stack* stack);
void exec_i64_eq(wasm_stack label, wasm_stack* stack);
void exec_i64_ne(wasm_stack label, wasm_stack* stack);
void exec_i64_lt_s(wasm_stack label, wasm_stack* stack);
void exec_i64_lt_u(wasm_stack label, wasm_stack* stack);
void exec_i64_gt_s(wasm_stack label, wasm_stack* stack);
void exec_i64_gt_u(wasm_stack label, wasm_stack* stack);
void exec_i64_le_s(wasm_stack label, wasm_stack* stack);
void exec_i64_le_u(wasm_stack label, wasm_stack* stack);
void exec_i64_ge_s(wasm_stack label, wasm_stack* stack);
void exec_i64_ge_u(wasm_stack label, wasm_stack* stack);
void exec_f32_eq(wasm_stack label, wasm_stack* stack);
void exec_f32_ne(wasm_stack label, wasm_stack* stack);
void exec_f32_lt(wasm_stack label, wasm_stack* stack);
void exec_f32_gt(wasm_stack label, wasm_stack* stack);
void exec_f32_le(wasm_stack label, wasm_stack* stack);
void exec_f32_ge(wasm_stack label, wasm_stack* stack);
void exec_f64_eq(wasm_stack label, wasm_stack* stack);
void exec_f64_ne(wasm_stack label, wasm_stack* stack);
void exec_f64_lt(wasm_stack label, wasm_stack* stack);
void exec_f64_gt(wasm_stack label, wasm_stack* stack);
void exec_f64_le(wasm_stack label, wasm_stack* stack);
void exec_f64_ge(wasm_stack label, wasm_stack* stack);
void exec_i32_clz(wasm_stack label, wasm_stack* stack);
void exec_i32_ctz(wasm_stack label, wasm_stack* stack);
void exec_i32_popcnt(wasm_stack label, wasm_stack* stack);
void exec_i32_add(wasm_stack label, wasm_stack* stack);
void exec_i32_sub(wasm_stack label, wasm_stack* stack);
void exec_i32_mul(wasm_stack label, wasm_stack* stack);
void exec_i32_div_s(wasm_stack label, wasm_stack* stack);
void exec_i32_div_u(wasm_stack label, wasm_stack* stack);
void exec_i32_rem_s(wasm_stack label, wasm_stack* stack);
void exec_i32_rem_u(wasm_stack label, wasm_stack* stack);
void exec_i32_and(wasm_stack label, wasm_stack* stack);
void exec_i32_or(wasm_stack label, wasm_stack* stack);
void exec_i32_xor(wasm_stack label, wasm_stack* stack);
void exec_i32_shl(wasm_stack label, wasm_stack* stack);
void exec_i32_shr_s(wasm_stack label, wasm_stack* stack);
void exec_i32_shr_u(wasm_stack label, wasm_stack* stack);
void exec_i32_rotl(wasm_stack label, wasm_stack* stack);
void exec_i32_rotr(wasm_stack label, wasm_stack* stack);
void exec_i64_clz(wasm_stack label, wasm_stack* stack);
void exec_i64_ctz(wasm_stack label, wasm_stack* stack);
void exec_i64_popcnt(wasm_stack label, wasm_stack* stack);
void exec_i64_add(wasm_stack label, wasm_stack* stack);
void exec_i64_sub(wasm_stack label, wasm_stack* stack);
void exec_i64_mul(wasm_stack label, wasm_stack* stack);
void exec_i64_div_s(wasm_stack label, wasm_stack* stack);
void exec_i64_div_u(wasm_stack label, wasm_stack* stack);
void exec_i64_rem_s(wasm_stack label, wasm_stack* stack);
void exec_i64_rem_u(wasm_stack label, wasm_stack* stack);
void exec_i64_and(wasm_stack label, wasm_stack* stack);
void exec_i64_or(wasm_stack label, wasm_stack* stack);
void exec_i64_xor(wasm_stack label, wasm_stack* stack);
void exec_i64_shl(wasm_stack label, wasm_stack* stack);
void exec_i64_shr_s(wasm_stack label, wasm_stack* stack);
void exec_i64_shr_u(wasm_stack label, wasm_stack* stack);
void exec_i64_rotl(wasm_stack label, wasm_stack* stack);
void exec_i64_rotr(wasm_stack label, wasm_stack* stack);
void exec_f32_abs(wasm_stack label, wasm_stack* stack);
void exec_f32_neg(wasm_stack label, wasm_stack* stack);
void exec_f32_ceil(wasm_stack label, wasm_stack* stack);
void exec_f32_floor(wasm_stack label, wasm_stack* stack);
void exec_f32_trunc(wasm_stack label, wasm_stack* stack);
void exec_f32_nearest(wasm_stack label, wasm_stack* stack);
void exec_f32_sqrt(wasm_stack label, wasm_stack* stack);
void exec_f32_add(wasm_stack label, wasm_stack* stack);
void exec_f32_sub(wasm_stack label, wasm_stack* stack);
void exec_f32_mul(wasm_stack label, wasm_stack* stack);
void exec_f32_div(wasm_stack label, wasm_stack* stack);
void exec_f32_min(wasm_stack label, wasm_stack* stack);
void exec_f32_max(wasm_stack label, wasm_stack* stack);
void exec_f32_copysign(wasm_stack label, wasm_stack* stack);
void exec_f64_abs(wasm_stack label, wasm_stack* stack);
void exec_f64_neg(wasm_stack label, wasm_stack* stack);
void exec_f64_ceil(wasm_stack label, wasm_stack* stack);
void exec_f64_floor(wasm_stack label, wasm_stack* stack);
void exec_f64_trunc(wasm_stack label, wasm_stack* stack);
void exec_f64_nearest(wasm_stack label, wasm_stack* stack);
void exec_f64_sqrt(wasm_stack label, wasm_stack* stack);
void exec_f64_add(wasm_stack label, wasm_stack* stack);
void exec_f64_sub(wasm_stack label, wasm_stack* stack);
void exec_f64_mul(wasm_stack label, wasm_stack* stack);
void exec_f64_div(wasm_stack label, wasm_stack* stack);
void exec_f64_min(wasm_stack label, wasm_stack* stack);
void exec_f64_max(wasm_stack label, wasm_stack* stack);
void exec_f64_copysign(wasm_stack label, wasm_stack* stack);
void exec_i32_wrap_i64(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_s_f32(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_u_f32(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_s_f64(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_u_f64(wasm_stack label, wasm_stack* stack);
void exec_i64_extend_s_i32(wasm_stack label, wasm_stack* stack);
void exec_i64_extend_u_i32(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_s_f32(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_u_f32(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_s_f64(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_u_f64(wasm_stack label, wasm_stack* stack);
void exec_f32_convert_s_i32(wasm_stack label, wasm_stack* stack);
void exec_f32_convert_u_i32(wasm_stack label, wasm_stack* stack);
void exec_f32_convert_s_i64(wasm_stack label, wasm_stack* stack);
void exec_f32_convert_u_i64(wasm_stack label, wasm_stack* stack);
void exec_f32_demote_f64(wasm_stack label, wasm_stack* stack);
void exec_f64_convert_s_i32(wasm_stack label, wasm_stack* stack);
void exec_f64_convert_u_i32(wasm_stack label, wasm_stack* stack);
void exec_f64_convert_s_i64(wasm_stack label, wasm_stack* stack);
void exec_f64_convert_u_i64(wasm_stack label, wasm_stack* stack);
void exec_f64_promote_f32(wasm_stack label, wasm_stack* stack);
void exec_i32_reinterpret_f32(wasm_stack label, wasm_stack* stack);
void exec_i64_reinterpret_f64(wasm_stack label, wasm_stack* stack);
void exec_f32_reinterpret_i32(wasm_stack label, wasm_stack* stack);
void exec_f64_reinterpret_i64(wasm_stack label, wasm_stack* stack);
void exec_i32_extend8_s(wasm_stack label, wasm_stack* stack);
void exec_i32_extend16_s(wasm_stack label, wasm_stack* stack);
void exec_i64_extend8_s(wasm_stack label, wasm_stack* stack);
void exec_i64_extend16_s(wasm_stack label, wasm_stack* stack);
void exec_i64_extend32_s(wasm_stack label, wasm_stack* stack);
void exec_ref_null(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_ref_is_null(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_ref_func(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_i32_trunc_sat_f32_s(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_sat_f32_u(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_sat_f64_s(wasm_stack label, wasm_stack* stack);
void exec_i32_trunc_sat_f64_u(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_sat_f32_s(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_sat_f32_u(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_sat_f64_s(wasm_stack label, wasm_stack* stack);
void exec_i64_trunc_sat_f64_u(wasm_stack label, wasm_stack* stack);
void exec_memory_init(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_data_drop(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_memory_copy(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_memory_fill(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_init(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_elem_drop(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_copy(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_grow(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_size(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:
void exec_table_fill(wasm_stack* label, wasm_stack* frame, wasm_stack* stack, wasm_store store); // TODO:

#endif