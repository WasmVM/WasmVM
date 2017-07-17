#ifndef NUMERIC_INST
#define NUMERIC_INST

#include "OperandStack.h"

namespace Instruction{
  // i32
  void i32_const (OperandStack &stack, uint32_t val);
  void i32_eqz (OperandStack &stack);
  void i32_eq (OperandStack &stack);
  void i32_ne (OperandStack &stack);
  void i32_lt_s (OperandStack &stack);
  void i32_gt_s (OperandStack &stack);
  void i32_le_s (OperandStack &stack);
  void i32_ge_s (OperandStack &stack);
  void i32_lt_u (OperandStack &stack);
  void i32_gt_u (OperandStack &stack);
  void i32_le_u (OperandStack &stack);
  void i32_ge_u (OperandStack &stack);
  void i32_clz (OperandStack &stack);
  void i32_ctz (OperandStack &stack);
  void i32_popcnt (OperandStack &stack);
  void i32_add (OperandStack &stack);
  void i32_sub (OperandStack &stack);
  void i32_mul (OperandStack &stack);
  void i32_div_u (OperandStack &stack);
  void i32_rem_u (OperandStack &stack);
  void i32_div_s (OperandStack &stack);
  void i32_rem_s (OperandStack &stack);
  void i32_and (OperandStack &stack);
  void i32_or (OperandStack &stack);
  void i32_xor (OperandStack &stack);
  void i32_shl (OperandStack &stack);
  void i32_shr_s (OperandStack &stack);
  void i32_shr_u (OperandStack &stack);
  void i32_rotl (OperandStack &stack);
  void i32_rotr (OperandStack &stack);
  // i64
  void i64_const (OperandStack &stack, uint64_t val);
  void i64_eqz (OperandStack &stack);
  void i64_eq (OperandStack &stack);
  void i64_ne (OperandStack &stack);
  void i64_lt_s (OperandStack &stack);
  void i64_gt_s (OperandStack &stack);
  void i64_le_s (OperandStack &stack);
  void i64_ge_s (OperandStack &stack);
  void i64_lt_u (OperandStack &stack);
  void i64_gt_u (OperandStack &stack);
  void i64_le_u (OperandStack &stack);
  void i64_ge_u (OperandStack &stack);
  void i64_clz (OperandStack &stack);
  void i64_ctz (OperandStack &stack);
  void i64_popcnt (OperandStack &stack);
  void i64_add (OperandStack &stack);
  void i64_sub (OperandStack &stack);
  void i64_mul (OperandStack &stack);
  void i64_div_u (OperandStack &stack);
  void i64_rem_u (OperandStack &stack);
  void i64_div_s (OperandStack &stack);
  void i64_rem_s (OperandStack &stack);
  void i64_and (OperandStack &stack);
  void i64_or (OperandStack &stack);
  void i64_xor (OperandStack &stack);
  void i64_shl (OperandStack &stack);
  void i64_shr_s (OperandStack &stack);
  void i64_shr_u (OperandStack &stack);
  void i64_rotl (OperandStack &stack);
  void i64_rotr (OperandStack &stack);
  // f32
  void f32_const ();
  void f32_eq ();
  void f32_ne ();
  void f32_lt ();
  void f32_gt ();
  void f32_le ();
  void f32_ge ();
  void f32_abs ();
  void f32_neg ();
  void f32_ceil ();
  void f32_floor ();
  void f32_trunc ();
  void f32_nearest ();
  void f32_sqrt ();
  void f32_add ();
  void f32_sub ();
  void f32_mul ();
  void f32_div ();
  void f32_min ();
  void f32_max ();
  void f32_copysign ();
  // f64
  void f64_const ();
  void f64_eq ();
  void f64_ne ();
  void f64_lt ();
  void f64_gt ();
  void f64_le ();
  void f64_ge ();
  void f64_abs ();
  void f64_neg ();
  void f64_ceil ();
  void f64_floor ();
  void f64_trunc ();
  void f64_nearest ();
  void f64_sqrt ();
  void f64_add ();
  void f64_sub ();
  void f64_mul ();
  void f64_div ();
  void f64_min ();
  void f64_max ();
  void f64_copysign ();
  // Conversion
  void i32_wrap_i64 (OperandStack &stack);
  void i64_extend_s_i32 (OperandStack &stack);
  void i64_extend_u_i32 (OperandStack &stack);
  void i32_trunc_s_f32 (OperandStack &stack);
  void i32_trunc_u_f32 (OperandStack &stack);
  void i32_trunc_s_f64 (OperandStack &stack);
  void i32_trunc_u_f64 (OperandStack &stack);
  void i64_trunc_s_f32 (OperandStack &stack);
  void i64_trunc_u_f32 (OperandStack &stack);
  void i64_trunc_s_f64 (OperandStack &stack);
  void i64_trunc_u_f64 (OperandStack &stack);
  void i32_reinterpret_f32 (OperandStack &stack);
  void i64_reinterpret_f64 (OperandStack &stack);
  void f32_reinterpret_i32 (OperandStack &stack);
  void f64_reinterpret_i64 (OperandStack &stack);
}

#endif