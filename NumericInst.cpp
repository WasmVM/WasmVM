#include "NumericInst.h"

/*** i32 ***/
void Instruction::i32_const (OperandStack &stack, uint32_t val){
  Value value;
  value.data.i32 = val;
  value.type = i32;
  stack.pushVal(value);
}
void Instruction::i32_eqz (OperandStack &stack){
  Value value = stack.popVal();
  value.data.i32 = (value.data.i32 == 0) ? 1 : 0;
  value.type = i32;
  stack.pushVal(value);
}
void Instruction::i32_eq (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 == val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_ne (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 != val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_lt_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 < val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_gt_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 > val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_le_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 <= val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_ge_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = (val1.data.i32 >= val2.data.i32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_lt_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = (val1.data.u32 < val2.data.u32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_gt_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = (val1.data.u32 > val2.data.u32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_le_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = (val1.data.u32 <= val2.data.u32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_ge_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = (val1.data.u32 >= val2.data.u32) ? 1 : 0;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_clz (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i32 = 0;
  ret.type = i32;
  if(val.data.i32 == 0){
    ret.data.i32 = 32;
  }else{
    if((val.data.i32 & 0xFFFF0000) == 0) {ret.data.i32 += 16; val.data.i32 <<= 16;}
    if((val.data.i32 & 0xFF000000) == 0) {ret.data.i32 += 8; val.data.i32 <<= 8;}
    if((val.data.i32 & 0xF0000000) == 0) {ret.data.i32 += 4; val.data.i32 <<= 4;}
    if((val.data.i32 & 0xC0000000) == 0) {ret.data.i32 += 2; val.data.i32 <<= 2;}
    if((val.data.i32 & 0x80000000) == 0) {ret.data.i32 += 1;}
  }
  stack.pushVal(ret);
}
void Instruction::i32_ctz (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i32 = 0;
  ret.type = i32;
  if(val.data.i32 == 0){
    ret.data.i32 = 32;
  }else{
    if((val.data.i32 & 0x0000FFFF) == 0) {ret.data.i32 += 16; val.data.i32 >>= 16;}
    if((val.data.i32 & 0x000000FF) == 0) {ret.data.i32 += 8; val.data.i32 >>= 8;}
    if((val.data.i32 & 0x0000000F) == 0) {ret.data.i32 += 4; val.data.i32 >>= 4;}
    if((val.data.i32 & 0x00000003) == 0) {ret.data.i32 += 2; val.data.i32 >>= 2;}
    if((val.data.i32 & 0x00000001) == 0) {ret.data.i32 += 1;}
  }
  stack.pushVal(ret);
}
void Instruction::i32_popcnt (OperandStack &stack){
  Value val = stack.popVal();
  val.data.i32 = (val.data.i32 & 0x55555555) + ((val.data.i32 >> 1) & 0x55555555);
  val.data.i32 = (val.data.i32 & 0x33333333) + ((val.data.i32 >> 2) & 0x33333333);
  val.data.i32 = (val.data.i32 & 0x0f0f0f0f) + ((val.data.i32 >> 4) & 0x0f0f0f0f);
  val.data.i32 = (val.data.i32 & 0x00ff00ff) + ((val.data.i32 >> 8) & 0x00ff00ff);
  val.data.i32 = (val.data.i32 & 0x0000ffff) + ((val.data.i32 >>16) & 0x0000ffff);
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_add (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 + val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_sub (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 - val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_mul (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 * val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_div_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = val1.data.u32 / val2.data.u32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_rem_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = val1.data.u32 % val2.data.u32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_div_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 / val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_rem_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 % val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_and (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 & val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_or (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 | val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_xor (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 ^ val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_shl (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 << val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_shr_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i32 = val1.data.i32 >> val2.data.i32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_shr_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u32 = val1.data.u32 >> val2.data.u32;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_rotl (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  if(val2.data.i32 == 0){stack.pushVal(val1); return;}
  // save remaining
  uint32_t remaining = 0, shift = val2.data.i32;
  if(shift >= 16) {remaining |= 0x0000FFFF; shift -= 16;}
  if(shift >= 8)  {remaining <<= 8; remaining |= 0x000000FF; shift -= 8;}
  if(shift >= 4)  {remaining <<= 4; remaining |= 0x0000000F; shift -= 4;}
  if(shift >= 2)  {remaining <<= 2; remaining |= 0x00000003; shift -= 2;}
  if(shift >= 1)  {remaining <<= 1; remaining |= 0x00000001;}
  remaining = (val1.data.i32 >> (32 - val2.data.i32)) & remaining;
  Value val;
  val.data.i32 = (val1.data.i32 << val2.data.i32) | remaining;
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i32_rotr (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  if(val2.data.i32 == 0){stack.pushVal(val1); return;}
  // save remaining
  uint32_t remaining = 0, shift = val2.data.i32;
  if(shift >= 16) {remaining |= 0x0000FFFF; shift -= 16;}
  if(shift >= 8)  {remaining <<= 8; remaining |= 0x000000FF; shift -= 8;}
  if(shift >= 4)  {remaining <<= 4; remaining |= 0x0000000F; shift -= 4;}
  if(shift >= 2)  {remaining <<= 2; remaining |= 0x00000003; shift -= 2;}
  if(shift >= 1)  {remaining <<= 1; remaining |= 0x00000001;}
  remaining = (val1.data.i32 & remaining) << (32 - val2.data.i32);
  Value val;
  val.data.i32 = (((uint32_t) val1.data.i32) >> val2.data.i32) | remaining;
  val.type = i32;
  stack.pushVal(val);
}

/*** i64 ***/
void Instruction::i64_const (OperandStack &stack, uint64_t val){
  Value value;
  value.data.u64 = val;
  value.type = i64;
  stack.pushVal(value);
}
void Instruction::i64_eqz (OperandStack &stack){
  Value value = stack.popVal();
  value.data.i64 = (value.data.i64 == 0) ? 1 : 0;
  value.type = i64;
  stack.pushVal(value);
}
void Instruction::i64_eq (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 == val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_ne (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 != val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_lt_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 < val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_gt_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 > val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_le_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 <= val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_ge_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = (val1.data.i64 >= val2.data.i64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_lt_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = (val1.data.u64 < val2.data.u64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_gt_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = (val1.data.u64 > val2.data.u64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_le_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = (val1.data.u64 <= val2.data.u64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_ge_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = (val1.data.u64 >= val2.data.u64) ? 1 : 0;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_clz (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i64 = 0;
  ret.type = i64;
  if(val.data.i64 == 0){
    ret.data.i64 = 64;
  }else{
    if((val.data.i64 & 0xFFFFFFFF00000000) == 0) {ret.data.i64 += 32; val.data.i64 <<= 32;}
    if((val.data.i64 & 0xFFFF000000000000) == 0) {ret.data.i64 += 16; val.data.i64 <<= 16;}
    if((val.data.i64 & 0xFF00000000000000) == 0) {ret.data.i64 += 8; val.data.i64 <<= 8;}
    if((val.data.i64 & 0xF000000000000000) == 0) {ret.data.i64 += 4; val.data.i64 <<= 4;}
    if((val.data.i64 & 0xC000000000000000) == 0) {ret.data.i64 += 2; val.data.i64 <<= 2;}
    if((val.data.i64 & 0x8000000000000000) == 0) {ret.data.i64 += 1;}
  }
  stack.pushVal(ret);
}
void Instruction::i64_ctz (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i64 = 0;
  ret.type = i64;
  if(val.data.i64 == 0){
    ret.data.i64 = 64;
  }else{
    if((val.data.i64 & 0x00000000FFFFFFFF) == 0) {ret.data.i64 += 32; val.data.i64 >>= 32;}
    if((val.data.i64 & 0x000000000000FFFF) == 0) {ret.data.i64 += 16; val.data.i64 >>= 16;}
    if((val.data.i64 & 0x00000000000000FF) == 0) {ret.data.i64 += 8; val.data.i64 >>= 8;}
    if((val.data.i64 & 0x000000000000000F) == 0) {ret.data.i64 += 4; val.data.i64 >>= 4;}
    if((val.data.i64 & 0x0000000000000003) == 0) {ret.data.i64 += 2; val.data.i64 >>= 2;}
    if((val.data.i64 & 0x0000000000000001) == 0) {ret.data.i64 += 1;}
  }
  stack.pushVal(ret);
}
void Instruction::i64_popcnt (OperandStack &stack){
  Value val = stack.popVal();
  val.data.i64 = (val.data.i64 & 0x5555555555555555) + ((val.data.i64 >> 1) & 0x5555555555555555);
  val.data.i64 = (val.data.i64 & 0x3333333333333333) + ((val.data.i64 >> 2) & 0x3333333333333333);
  val.data.i64 = (val.data.i64 & 0x0f0f0f0f0f0f0f0f) + ((val.data.i64 >> 4) & 0x0f0f0f0f0f0f0f0f);
  val.data.i64 = (val.data.i64 & 0x00ff00ff00ff00ff) + ((val.data.i64 >> 8) & 0x00ff00ff00ff00ff);
  val.data.i64 = (val.data.i64 & 0x0000ffff0000ffff) + ((val.data.i64 >>16) & 0x0000ffff0000ffff);
  val.data.i64 = (val.data.i64 & 0x00000000ffffffff) + ((val.data.i64 >>32) & 0x00000000ffffffff);
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_add (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 + val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_sub (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 - val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_mul (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 * val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_div_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = val1.data.u64 / val2.data.u64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_rem_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = val1.data.u64 % val2.data.u64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_div_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 / val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_rem_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 % val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_and (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 & val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_or (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 | val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_xor (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 ^ val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_shl (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 << val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_shr_s (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.i64 = val1.data.i64 >> val2.data.i64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_shr_u (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  Value val;
  val.data.u64 = val1.data.u64 >> val2.data.u64;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_rotl (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  if(val2.data.i64 == 0){stack.pushVal(val1); return;}
  // save remaining
  uint64_t remaining = 0, shift = val2.data.i64;
  if(shift >= 32) {remaining |= 0x00000000FFFFFFFF; shift -= 32;}
  if(shift >= 16) {remaining <<= 16; remaining |= 0x000000000000FFFF; shift -= 16;}
  if(shift >= 8)  {remaining <<= 8;  remaining |= 0x00000000000000FF; shift -= 8;}
  if(shift >= 4)  {remaining <<= 4;  remaining |= 0x000000000000000F; shift -= 4;}
  if(shift >= 2)  {remaining <<= 2;  remaining |= 0x0000000000000003; shift -= 2;}
  if(shift >= 1)  {remaining <<= 1;  remaining |= 0x0000000000000001;}
  remaining = (val1.data.i64 >> (64 - val2.data.i64)) & remaining;
  Value val;
  val.data.i64 = (val1.data.i64 << val2.data.i64) | remaining;
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::i64_rotr (OperandStack &stack){
  Value val2 = stack.popVal();
  Value val1 = stack.popVal();
  if(val2.data.i64 == 0){stack.pushVal(val1); return;}
  // save remaining
  uint64_t remaining = 0, shift = val2.data.i64;
  if(shift >= 32) {remaining |= 0x00000000FFFFFFFF; shift -= 32;}
  if(shift >= 16) {remaining <<= 16; remaining |= 0x000000000000FFFF; shift -= 16;}
  if(shift >= 8)  {remaining <<= 8;  remaining |= 0x00000000000000FF; shift -= 8;}
  if(shift >= 4)  {remaining <<= 4;  remaining |= 0x000000000000000F; shift -= 4;}
  if(shift >= 2)  {remaining <<= 2;  remaining |= 0x0000000000000003; shift -= 2;}
  if(shift >= 1)  {remaining <<= 1;  remaining |= 0x0000000000000001;}
  remaining = (val1.data.i64 & remaining) << (64 - val2.data.i64);
  Value val;
  val.data.i64 = (((uint64_t) val1.data.i64) >> val2.data.i64) | remaining;
  val.type = i64;
  stack.pushVal(val);
}

/*** f32 ***/
void Instruction::f32_const (){
}
void Instruction::f32_eq (){
}
void Instruction::f32_ne (){
}
void Instruction::f32_lt (){
}
void Instruction::f32_gt (){
}
void Instruction::f32_le (){
}
void Instruction::f32_ge (){
}
void Instruction::f32_abs (){
}
void Instruction::f32_neg (){
}
void Instruction::f32_ceil (){
}
void Instruction::f32_floor (){
}
void Instruction::f32_trunc (){
}
void Instruction::f32_nearest (){
}
void Instruction::f32_sqrt (){
}
void Instruction::f32_add (){
}
void Instruction::f32_sub (){
}
void Instruction::f32_mul (){
}
void Instruction::f32_div (){
}
void Instruction::f32_min (){
}
void Instruction::f32_max (){
}
void Instruction::f32_copysign (){
}

/*** f64 ***/
void Instruction::f64_const (){
}
void Instruction::f64_eq (){
}
void Instruction::f64_ne (){
}
void Instruction::f64_lt (){
}
void Instruction::f64_gt (){
}
void Instruction::f64_le (){
}
void Instruction::f64_ge (){
}
void Instruction::f64_abs (){
}
void Instruction::f64_neg (){
}
void Instruction::f64_ceil (){
}
void Instruction::f64_floor (){
}
void Instruction::f64_trunc (){
}
void Instruction::f64_nearest (){
}
void Instruction::f64_sqrt (){
}
void Instruction::f64_add (){
}
void Instruction::f64_sub (){
}
void Instruction::f64_mul (){
}
void Instruction::f64_div (){
}
void Instruction::f64_min (){
}
void Instruction::f64_max (){
}
void Instruction::f64_copysign (){
}

/*** Conversion ***/
void Instruction::i32_wrap_i64 (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i32 = (int32_t)val.data.i64;
  ret.type = i32;
  stack.pushVal(ret);
}
void Instruction::i64_extend_s_i32 (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  if(val.data.i32 & 0x80000000){
    ret.data.i64 = 0xFFFFFFFF00000000;
  }else{
    ret.data.i64 = 0;
  }
  ret.data.i64 |= (int64_t) val.data.i32 & 0x00000000FFFFFFFF;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i64_extend_u_i32 (OperandStack &stack){
  Value val = stack.popVal();
  Value ret;
  ret.data.i64 = 0;
  ret.data.i64 |= (int64_t) val.data.i32 & 0x00000000FFFFFFFF;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i32_trunc_s_f32 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i32 & 0x7F800000) == 0x7F800000){
    if(val.data.i32 & 0x007FFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i32 = (int32_t) val.data.f32;
  ret.type = i32;
  stack.pushVal(ret);
}
void Instruction::i32_trunc_u_f32 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i32 & 0x7F800000) == 0x7F800000){
    if(val.data.i32 & 0x007FFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i32 = (int32_t) val.data.f32;
  ret.data.i32 *= (-2) * (ret.data.i32 < 0) + 1;
  ret.type = i32;
  stack.pushVal(ret);
}
void Instruction::i32_trunc_s_f64 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i64 & 0x7FF0000000000000) == 0x7FF0000000000000){
    if(val.data.i64 & 0x000FFFFFFFFFFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i32 = (int32_t) val.data.f64;
  ret.type = i32;
  stack.pushVal(ret);
}
void Instruction::i32_trunc_u_f64 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i64 & 0x7FF0000000000000) == 0x7FF0000000000000){
    if(val.data.i64 & 0x000FFFFFFFFFFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i32 = (int32_t) val.data.f64;
  ret.data.i32 *= (-2) * (ret.data.i32 < 0) + 1;
  ret.type = i32;
  stack.pushVal(ret);
}
void Instruction::i32_reinterpret_f32 (OperandStack &stack){
  Value val = stack.popVal();
  if(val.type != f32){
    throw "Wrong type to reinterpret from f32 to i32";
  }
  val.type = i32;
  stack.pushVal(val);
}
void Instruction::i64_trunc_s_f32 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i32 & 0x7F800000) == 0x7F800000){
    if(val.data.i32 & 0x007FFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i64 = (int64_t) val.data.f32;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i64_trunc_u_f32 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i32 & 0x7F800000) == 0x7F800000){
    if(val.data.i32 & 0x007FFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i64 = (int64_t) val.data.f32;
  ret.data.i64 *= (-2) * (ret.data.i64 < 0) + 1;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i64_trunc_s_f64 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i64 & 0x7FF0000000000000) == 0x7FF0000000000000){
    if(val.data.i64 & 0x000FFFFFFFFFFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i64 = (int64_t) val.data.f64;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i64_trunc_u_f64 (OperandStack &stack){
  Value val = stack.popVal();
  if((val.data.i64 & 0x7FF0000000000000) == 0x7FF0000000000000){
    if(val.data.i64 & 0x000FFFFFFFFFFFFF){ // NaN
      throw "Can't truncate NaN";
    }else{
      throw "Can't truncate inf";
    }
  }
  Value ret;
  ret.data.i64 = (int64_t) val.data.f64;
  ret.data.i64 *= (-2) * (ret.data.i64 < 0) + 1;
  ret.type = i64;
  stack.pushVal(ret);
}
void Instruction::i64_reinterpret_f64 (OperandStack &stack){
  Value val = stack.popVal();
  if(val.type != f64){
    throw "Wrong type to reinterpret from f64 to i64";
  }
  val.type = i64;
  stack.pushVal(val);
}
void Instruction::f32_reinterpret_i32 (OperandStack &stack){
  Value val = stack.popVal();
  if(val.type != i32){
    throw "Wrong type to reinterpret from i32 to f32";
  }
  val.type = f32;
  stack.pushVal(val);
}
void Instruction::f64_reinterpret_i64 (OperandStack &stack){
  Value val = stack.popVal();
  if(val.type != i64){
    throw "Wrong type to reinterpret from i64 to f64";
  }
  val.type = f64;
  stack.pushVal(val);
}