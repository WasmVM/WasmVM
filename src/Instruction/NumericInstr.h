#ifndef NUMERICINSTR_DEF
#define NUMERICINSTR_DEF

#include <cstdint>
#include <cmath>
#include <Stack.h>
#include <Instance.h>
#include <Util.h>

namespace Instruction {
void i32_const(Stack& coreStack, std::int32_t val);
void i64_const(Stack& coreStack, std::int64_t val);
void f32_const(Stack& coreStack, float val);
void f64_const(Stack& coreStack, double val);

void i32_eqz(Stack& coreStack);
void i32_eq(Stack& coreStack);
void i32_ne(Stack& coreStack);
void i32_lt_s(Stack& coreStack);
void i32_lt_u(Stack& coreStack);
void i32_gt_s(Stack& coreStack);
void i32_gt_u(Stack& coreStack);
void i32_le_s(Stack& coreStack);
void i32_le_u(Stack& coreStack);
void i32_ge_s(Stack& coreStack);
void i32_ge_u(Stack& coreStack);
void i32_clz(Stack& coreStack);
void i32_ctz(Stack& coreStack);
void i32_popcnt(Stack& coreStack);
void i32_add(Stack& coreStack);
void i32_sub(Stack& coreStack);
void i32_mul(Stack& coreStack);
void i32_div_s(Stack& coreStack);
void i32_div_u(Stack& coreStack);
void i32_rem_s(Stack& coreStack);
void i32_rem_u(Stack& coreStack);
void i32_and(Stack& coreStack);
void i32_or(Stack& coreStack);
void i32_xor(Stack& coreStack);
void i32_shl(Stack& coreStack);
void i32_shr_s(Stack& coreStack);
void i32_shr_u(Stack& coreStack);
void i32_rotl(Stack& coreStack);
void i32_rotr(Stack& coreStack);

void i64_eqz(Stack& coreStack);
void i64_eq(Stack& coreStack);
void i64_ne(Stack& coreStack);
void i64_lt_s(Stack& coreStack);
void i64_lt_u(Stack& coreStack);
void i64_gt_s(Stack& coreStack);
void i64_gt_u(Stack& coreStack);
void i64_le_s(Stack& coreStack);
void i64_le_u(Stack& coreStack);
void i64_ge_s(Stack& coreStack);
void i64_ge_u(Stack& coreStack);
void i64_clz(Stack& coreStack);
void i64_ctz(Stack& coreStack);
void i64_popcnt(Stack& coreStack);
void i64_add(Stack& coreStack);
void i64_sub(Stack& coreStack);
void i64_mul(Stack& coreStack);
void i64_div_s(Stack& coreStack);
void i64_div_u(Stack& coreStack);
void i64_rem_s(Stack& coreStack);
void i64_rem_u(Stack& coreStack);
void i64_and(Stack& coreStack);
void i64_or(Stack& coreStack);
void i64_xor(Stack& coreStack);
void i64_shl(Stack& coreStack);
void i64_shr_s(Stack& coreStack);
void i64_shr_u(Stack& coreStack);
void i64_rotl(Stack& coreStack);
void i64_rotr(Stack& coreStack);

void f32_eq(Stack& coreStack);
void f32_ne(Stack& coreStack);
void f32_lt(Stack& coreStack);
void f32_gt(Stack& coreStack);
void f32_le(Stack& coreStack);
void f32_ge(Stack& coreStack);
void f32_abs(Stack& coreStack);
void f32_neg(Stack& coreStack);
void f32_ceil(Stack& coreStack);
void f32_floor(Stack& coreStack);
void f32_trunc(Stack& coreStack);
void f32_nearest(Stack& coreStack);
void f32_sqrt(Stack& coreStack);
void f32_add(Stack& coreStack);
void f32_sub(Stack& coreStack);
void f32_mul(Stack& coreStack);
void f32_div(Stack& coreStack);
void f32_min(Stack& coreStack);
void f32_max(Stack& coreStack);
void f32_copysign(Stack& coreStack);

void f64_eq(Stack& coreStack);
void f64_ne(Stack& coreStack);
void f64_lt(Stack& coreStack);
void f64_gt(Stack& coreStack);
void f64_le(Stack& coreStack);
void f64_ge(Stack& coreStack);
void f64_abs(Stack& coreStack);
void f64_neg(Stack& coreStack);
void f64_ceil(Stack& coreStack);
void f64_floor(Stack& coreStack);
void f64_trunc(Stack& coreStack);
void f64_nearest(Stack& coreStack);
void f64_sqrt(Stack& coreStack);
void f64_add(Stack& coreStack);
void f64_sub(Stack& coreStack);
void f64_mul(Stack& coreStack);
void f64_div(Stack& coreStack);
void f64_min(Stack& coreStack);
void f64_max(Stack& coreStack);
void f64_copysign(Stack& coreStack);

void i32_wrap_i64(Stack& coreStack);
void i32_trunc_s_f32(Stack& coreStack);
void i32_trunc_u_f32(Stack& coreStack);
void i32_trunc_s_f64(Stack& coreStack);
void i32_trunc_u_f64(Stack& coreStack);
void i64_extend_s_i32(Stack& coreStack);
void i64_extend_u_i32(Stack& coreStack);
void i64_trunc_s_f32(Stack& coreStack);
void i64_trunc_u_f32(Stack& coreStack);
void i64_trunc_s_f64(Stack& coreStack);
void i64_trunc_u_f64(Stack& coreStack);
void f32_convert_s_i32(Stack& coreStack);
void f32_convert_u_i32(Stack& coreStack);
void f32_convert_s_i64(Stack& coreStack);
void f32_convert_u_i64(Stack& coreStack);
void f32_demote_f64(Stack& coreStack);
void f64_convert_s_i32(Stack& coreStack);
void f64_convert_u_i32(Stack& coreStack);
void f64_convert_s_i64(Stack& coreStack);
void f64_convert_u_i64(Stack& coreStack);
void f64_promote_f32(Stack& coreStack);
void i32_reinterpret_f32(Stack& coreStack);
void i64_reinterpret_f64(Stack& coreStack);
void f32_reinterpret_i32(Stack& coreStack);
void f64_reinterpret_i64(Stack& coreStack);
}

#endif