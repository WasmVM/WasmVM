// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"
#include "numeric.hpp"
#include <bit>
#include <cmath>
#include <limits>

using namespace WasmVM;

void RunVisitor::operator()(Instr::I32_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::I32_eqz&){
    put_op(stack, (i32_t)(get_op<i32_t>(stack) == 0));
}
void RunVisitor::operator()(Instr::I32_eq&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first == ops.second));
}
void RunVisitor::operator()(Instr::I32_ne&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first != ops.second));
}
void RunVisitor::operator()(Instr::I32_lt_s&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first < ops.second));
}
void RunVisitor::operator()(Instr::I32_lt_u&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first < (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_gt_s&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first > ops.second));
}
void RunVisitor::operator()(Instr::I32_gt_u&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first > (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_le_s&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first <= ops.second));
}
void RunVisitor::operator()(Instr::I32_le_u&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first <= (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_ge_s&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)(ops.first >= ops.second));
}
void RunVisitor::operator()(Instr::I32_ge_u&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first >= (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_clz&){
    put_op(stack, (i32_t)std::countl_zero((u32_t)get_op<i32_t>(stack)));
}
void RunVisitor::operator()(Instr::I32_ctz&){
    put_op(stack, (i32_t)std::countr_zero((u32_t)get_op<i32_t>(stack)));
}
void RunVisitor::operator()(Instr::I32_popcnt&){
    put_op(stack, (i32_t)std::popcount((u32_t)get_op<i32_t>(stack)));
}
void RunVisitor::operator()(Instr::I32_add&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, ops.first + ops.second);
}
void RunVisitor::operator()(Instr::I32_sub&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, ops.first - ops.second);
}
void RunVisitor::operator()(Instr::I32_mul&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, ops.first * ops.second);
}
// The integer division opcodes trap instead of reaching C++ `/` and `%` with
// operands those are not defined for. Both guarded cases are *undefined
// behavior* in C++, not merely wrong answers: on x86-64 `idiv` raises #DE, so
// the host process dies of SIGFPE and takes the embedder down with it. The
// spec makes them wasm traps, which an embedder can catch.
void RunVisitor::operator()(Instr::I32_div_s&){
    auto ops = get_ops<i32_t>(stack);
    if(ops.second == 0){
        throw Exception::integer_divide_by_zero();
    }
    // INT32_MIN / -1 is +2^31, one past the top of the signed range.
    if((ops.first == std::numeric_limits<i32_t>::min()) && (ops.second == -1)){
        throw Exception::integer_overflow();
    }
    put_op(stack, ops.first / ops.second);
}
void RunVisitor::operator()(Instr::I32_div_u&){
    auto ops = get_ops<i32_t>(stack);
    if(ops.second == 0){
        throw Exception::integer_divide_by_zero();
    }
    put_op(stack, (i32_t)((u32_t)ops.first / (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_rem_s&){
    auto ops = get_ops<i32_t>(stack);
    if(ops.second == 0){
        throw Exception::integer_divide_by_zero();
    }
    // INT32_MIN % -1 is 0 and does *not* trap, unlike div_s -- but evaluating
    // it through C++ `%` is the same UB as the division, so answer directly.
    if((ops.first == std::numeric_limits<i32_t>::min()) && (ops.second == -1)){
        put_op(stack, (i32_t)0);
        return;
    }
    put_op(stack, ops.first % ops.second);
}
void RunVisitor::operator()(Instr::I32_rem_u&){
    auto ops = get_ops<i32_t>(stack);
    if(ops.second == 0){
        throw Exception::integer_divide_by_zero();
    }
    put_op(stack, (i32_t)((u32_t)ops.first % (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_and&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first & (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_or&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first | (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_xor&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first ^ (u32_t)ops.second));
}
// Shift counts are taken modulo 32 (spec ishl / ishr); C++ leaves a count >= width undefined.
void RunVisitor::operator()(Instr::I32_shl&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first << ((u32_t)ops.second & 31)));
}
void RunVisitor::operator()(Instr::I32_shr_s&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, ops.first >> ((u32_t)ops.second & 31));
}
void RunVisitor::operator()(Instr::I32_shr_u&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)((u32_t)ops.first >> ((u32_t)ops.second & 31)));
}
void RunVisitor::operator()(Instr::I32_rotl&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)std::rotl((u32_t)ops.first, (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_rotr&){
    auto ops = get_ops<i32_t>(stack);
    put_op(stack, (i32_t)std::rotr((u32_t)ops.first, (u32_t)ops.second));
}
void RunVisitor::operator()(Instr::I32_reinterpret_f32&){
    f32_t value = get_op<f32_t>(stack);
    put_op(stack, *reinterpret_cast<i32_t*>(&value));
}
void RunVisitor::operator()(Instr::I32_extend8_s&){
    put_op(stack, (i32_t)(i8_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::I32_extend16_s&){
    put_op(stack, (i32_t)(i16_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f32_s&){
    f32_t value = get_op<f32_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i32_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i32_t)((value < 0) ? INT32_MIN : INT32_MAX));
    }else if(value < (f32_t)INT32_MIN){
        put_op(stack, (i32_t)INT32_MIN);
    }else if(value > (f32_t)INT32_MAX){
        put_op(stack, (i32_t)INT32_MAX);
    }else{
        put_op(stack, (i32_t)value);
    }
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f32_u&){
    f32_t value = get_op<f32_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i32_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i32_t)((value < 0) ? 0 : UINT32_MAX));
    }else if(value < 0){
        put_op(stack, (i32_t)0);
    }else if(value > (f32_t)UINT32_MAX){
        put_op(stack, (i32_t)UINT32_MAX);
    }else{
        put_op(stack, (i32_t)value);
    }
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f64_s&){
    f64_t value = get_op<f64_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i32_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i32_t)((value < 0) ? INT32_MIN : INT32_MAX));
    }else if(value < (f64_t)INT32_MIN){
        put_op(stack, (i32_t)INT32_MIN);
    }else if(value > (f64_t)INT32_MAX){
        put_op(stack, (i32_t)INT32_MAX);
    }else{
        put_op(stack, (i32_t)value);
    }
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f64_u&){
    f64_t value = get_op<f64_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i32_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i32_t)((value < 0) ? 0 : UINT32_MAX));
    }else if(value < 0){
        put_op(stack, (i32_t)0);
    }else if(value > (f64_t)UINT32_MAX){
        put_op(stack, (i32_t)UINT32_MAX);
    }else{
        put_op(stack, (i32_t)value);
    }
}
void RunVisitor::operator()(Instr::I32_wrap_i64&){
    put_op(stack, (i32_t)get_op<i64_t>(stack));
}
// Non-saturating float-to-integer truncation traps (rather than saturating)
// when the operand is NaN, infinite, or truncates outside the target range.
// The range bounds reflect truncation toward zero: e.g. for signed i32 the
// valid open interval is (-2^31 - 1, 2^31), so 2147483647.9 is in range but
// 2147483648.0 traps. NaN and infinities fail every comparison below, so they
// fall through to the trap automatically.
void RunVisitor::operator()(Instr::I32_trunc_f32_s&){
    f32_t value = get_op<f32_t>(stack);
    if(!(value > -2147483649.0 && value < 2147483648.0)){ // (-2^31 - 1, 2^31)
        throw Exception::invalid_conversion_to_integer();
    }
    put_op(stack, (i32_t)value);
}
void RunVisitor::operator()(Instr::I32_trunc_f32_u&){
    f32_t value = get_op<f32_t>(stack);
    if(!(value > -1.0 && value < 4294967296.0)){ // (-1, 2^32)
        throw Exception::invalid_conversion_to_integer();
    }
    put_op(stack, (i32_t)(u32_t)value);
}
void RunVisitor::operator()(Instr::I32_trunc_f64_s&){
    f64_t value = get_op<f64_t>(stack);
    if(!(value > -2147483649.0 && value < 2147483648.0)){ // (-2^31 - 1, 2^31)
        throw Exception::invalid_conversion_to_integer();
    }
    put_op(stack, (i32_t)value);
}
void RunVisitor::operator()(Instr::I32_trunc_f64_u&){
    f64_t value = get_op<f64_t>(stack);
    if(!(value > -1.0 && value < 4294967296.0)){ // (-1, 2^32)
        throw Exception::invalid_conversion_to_integer();
    }
    put_op(stack, (i32_t)(u64_t)value);
}