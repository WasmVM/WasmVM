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

void RunVisitor::operator()(Instr::I64_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::I64_eqz&){
    put_op(stack, (i64_t)(get_op<i64_t>(stack) == 0));
}
void RunVisitor::operator()(Instr::I64_eq&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first == ops.second));
}
void RunVisitor::operator()(Instr::I64_ne&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first != ops.second));
}
void RunVisitor::operator()(Instr::I64_lt_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first < ops.second));
}
void RunVisitor::operator()(Instr::I64_lt_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first < (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_gt_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first > ops.second));
}
void RunVisitor::operator()(Instr::I64_gt_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first > (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_le_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first <= ops.second));
}
void RunVisitor::operator()(Instr::I64_le_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first <= (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_ge_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)(ops.first >= ops.second));
}
void RunVisitor::operator()(Instr::I64_ge_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first >= (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_clz&){
    put_op(stack, (i64_t)std::countl_zero((u64_t)get_op<i64_t>(stack)));
}
void RunVisitor::operator()(Instr::I64_ctz&){
    put_op(stack, (i64_t)std::countr_zero((u64_t)get_op<i64_t>(stack)));
}
void RunVisitor::operator()(Instr::I64_popcnt&){
    put_op(stack, (i64_t)std::popcount((u64_t)get_op<i64_t>(stack)));
}
void RunVisitor::operator()(Instr::I64_add&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first + ops.second);
}
void RunVisitor::operator()(Instr::I64_sub&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first - ops.second);
}
void RunVisitor::operator()(Instr::I64_mul&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first * ops.second);
}
void RunVisitor::operator()(Instr::I64_div_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first / ops.second);
}
void RunVisitor::operator()(Instr::I64_div_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first / (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_rem_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first % ops.second);
}
void RunVisitor::operator()(Instr::I64_rem_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first % (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_and&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first & (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_or&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first | (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_xor&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first ^ (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_shl&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first << (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_shr_s&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, ops.first >> ops.second);
}
void RunVisitor::operator()(Instr::I64_shr_u&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)((u64_t)ops.first >> (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_rotl&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)std::rotl((u64_t)ops.first, (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_rotr&){
    auto ops = get_ops<i64_t>(stack);
    put_op(stack, (i64_t)std::rotr((u64_t)ops.first, (u64_t)ops.second));
}
void RunVisitor::operator()(Instr::I64_extend_i32_s&){
    put_op(stack, (i64_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::I64_extend_i32_u&){
    put_op(stack, (i64_t)(u32_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::I64_trunc_f32_s&){
    put_op(stack, (i64_t)get_op<f32_t>(stack));
}
void RunVisitor::operator()(Instr::I64_trunc_f32_u&){
    put_op(stack, (i64_t)(u32_t)get_op<f32_t>(stack));
}
void RunVisitor::operator()(Instr::I64_trunc_f64_s&){
    put_op(stack, (i64_t)get_op<f64_t>(stack));
}
void RunVisitor::operator()(Instr::I64_trunc_f64_u&){
    put_op(stack, (i64_t)(u64_t)get_op<f64_t>(stack));
}
void RunVisitor::operator()(Instr::I64_reinterpret_f64&){
    f64_t value = get_op<f64_t>(stack);
    put_op(stack, *reinterpret_cast<i64_t*>(&value));
}
void RunVisitor::operator()(Instr::I64_extend8_s&){
    put_op(stack, (i64_t)(i8_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::I64_extend16_s&){
    put_op(stack, (i64_t)(i16_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::I64_extend32_s&){
    put_op(stack, (i64_t)(i32_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f32_s&){
    f32_t value = get_op<f32_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i64_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i64_t)((value < 0) ? INT64_MIN : INT64_MAX));
    }else if(value < (f32_t)INT64_MIN){
        put_op(stack, (i64_t)INT64_MIN);
    }else if(value > (f32_t)INT64_MAX){
        put_op(stack, (i64_t)INT64_MAX);
    }else{
        put_op(stack, (i64_t)value);
    }
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f32_u&){
    f32_t value = get_op<f32_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i64_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i64_t)((value < 0) ? 0 : UINT64_MAX));
    }else if(value < 0){
        put_op(stack, (i64_t)0);
    }else if(value > (f32_t)UINT64_MAX){
        put_op(stack, (i64_t)UINT64_MAX);
    }else{
        put_op(stack, (i64_t)value);
    }
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f64_s&){
    f64_t value = get_op<f64_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i64_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i64_t)((value < 0) ? INT64_MIN : INT64_MAX));
    }else if(value < (f64_t)INT64_MIN){
        put_op(stack, (i64_t)INT64_MIN);
    }else if(value > (f64_t)INT64_MAX){
        put_op(stack, (i64_t)INT64_MAX);
    }else{
        put_op(stack, (i64_t)value);
    }
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f64_u&){
    f64_t value = get_op<f64_t>(stack);
    if(std::isnan(value)){
        put_op(stack, (i64_t)0);
    }else if(std::isinf(value)){
        put_op(stack, (i64_t)((value < 0) ? 0 : UINT64_MAX));
    }else if(value < 0){
        put_op(stack, (i64_t)0);
    }else if(value > (f64_t)UINT64_MAX){
        put_op(stack, (i64_t)UINT64_MAX);
    }else{
        put_op(stack, (i64_t)value);
    }
}