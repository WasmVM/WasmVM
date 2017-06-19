#ifndef NUMERIC_INST
#define NUMERIC_INST

#include "OperandStack.h"

namespace Instruction{
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
  void i32_wrap_i64 (OperandStack &stack);
  void i64_extend_s_i32 (OperandStack &stack);
  void i64_extend_u_i32 (OperandStack &stack);
}

#endif