// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"
#include "numeric.hpp"
#include <cmath>

using namespace WasmVM;

void RunVisitor::operator()(Instr::F32_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::F32_eq&){
    auto ops = get_ops<f32_t>(stack);
    put_op(stack, (i32_t)(ops.first == ops.second));
}
void RunVisitor::operator()(Instr::F32_ne&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_lt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_gt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_le&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_ge&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_abs&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_neg&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_ceil&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_floor&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_trunc&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_nearest&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_sqrt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_add&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_sub&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_mul&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_div&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_min&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_max&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_copysign&){
    // TODO:
}
void RunVisitor::operator()(Instr::F32_reinterpret_i32&){
    i32_t value = get_op<i32_t>(stack);
    put_op(stack, *reinterpret_cast<f32_t*>(&value));
}
void RunVisitor::operator()(Instr::F32_convert_s_i32&){
    put_op(stack, (f32_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::F32_convert_u_i32&){
    put_op(stack, (f32_t)(u32_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::F32_convert_s_i64&){
    put_op(stack, (f32_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::F32_convert_u_i64&){
    put_op(stack, (f32_t)(u64_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::F32_demote_f64&){
    f64_t value = get_op<f64_t>(stack);
    if(std::isnan(value)){
        u64_t val64 = *reinterpret_cast<u64_t*>(&value);
        u32_t result = ((val64 >> 63) ? 0x80000000UL : 0);
        if((val64 & 0x7fffffffffffffffULL) == 0x7ff8000000000000ULL){
            // canonical nan
            result |= 0x7fc00000UL;
        }else{
            result |= 0x7f800001UL;
        }
        put_op(stack, *reinterpret_cast<f32_t*>(&result));
    }else{
        put_op(stack, (f32_t)value);
    }
}