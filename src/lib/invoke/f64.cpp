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

void RunVisitor::operator()(Instr::F64_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::F64_eq&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)0);
    }else if(std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO){
        put_op(stack, (i32_t)1);
    }else{
        put_op(stack, (i32_t)(ops.first == ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_ne&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)1);
    }else if(std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO){
        put_op(stack, (i32_t)0);
    }else{
        put_op(stack, (i32_t)(ops.first != ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_lt&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)
        || ((std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO))
        || (ops.first == ops.second)
    ){
        put_op(stack, (i32_t)0);
    }else if(std::fpclassify(ops.first) == FP_INFINITE){
        put_op(stack, (i32_t)std::signbit(ops.first));
    }else if(std::fpclassify(ops.second) == FP_INFINITE){
        put_op(stack, (i32_t)!std::signbit(ops.second));
    }else{
        put_op(stack, (i32_t)(ops.first < ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_gt&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)
        || ((std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO))
        || (ops.first == ops.second)
    ){
        put_op(stack, (i32_t)0);
    }else if(std::fpclassify(ops.first) == FP_INFINITE){
        put_op(stack, (i32_t)!std::signbit(ops.first));
    }else if(std::fpclassify(ops.second) == FP_INFINITE){
        put_op(stack, (i32_t)std::signbit(ops.second));
    }else{
        put_op(stack, (i32_t)(ops.first > ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_le&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)0);
    }else if((std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO)
        || (ops.first == ops.second)
    ){
        put_op(stack, (i32_t)1);
    }else if(std::fpclassify(ops.first) == FP_INFINITE){
        put_op(stack, (i32_t)std::signbit(ops.first));
    }else if(std::fpclassify(ops.second) == FP_INFINITE){
        put_op(stack, (i32_t)!std::signbit(ops.second));
    }else{
        put_op(stack, (i32_t)(ops.first <= ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_ge&){
    auto ops = get_ops<f64_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)0);
    }else if((std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO)
        || (ops.first == ops.second)
    ){
        put_op(stack, (i32_t)1);
    }else if(std::fpclassify(ops.first) == FP_INFINITE){
        put_op(stack, (i32_t)!std::signbit(ops.first));
    }else if(std::fpclassify(ops.second) == FP_INFINITE){
        put_op(stack, (i32_t)std::signbit(ops.second));
    }else{
        put_op(stack, (i32_t)(ops.first >= ops.second));
    }
}
void RunVisitor::operator()(Instr::F64_abs&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_neg&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_ceil&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_floor&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_trunc&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_nearest&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_sqrt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_add&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_sub&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_mul&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_div&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_min&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_max&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_copysign&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_reinterpret_i64&){
    i64_t value = get_op<i64_t>(stack);
    put_op(stack, *reinterpret_cast<f64_t*>(&value));
}
void RunVisitor::operator()(Instr::F64_convert_s_i32&){
    put_op(stack, (f64_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::F64_convert_u_i32&){
    put_op(stack, (f64_t)(u64_t)get_op<i32_t>(stack));
}
void RunVisitor::operator()(Instr::F64_convert_s_i64&){
    put_op(stack, (f64_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::F64_convert_u_i64&){
    put_op(stack, (f64_t)(u64_t)get_op<i64_t>(stack));
}
void RunVisitor::operator()(Instr::F64_promote_f32&){
    f32_t value = get_op<f32_t>(stack);
    if(std::isnan(value)){
        u64_t result = (std::signbit(value) ? 0x8000000000000000ULL : 0);
        if((*reinterpret_cast<u32_t*>(&value) & 0x7fffffffUL) == 0x7fc00000UL){
            // canonical nan
            result |= 0x7ff8000000000000ULL;
        }else{
            result |= 0x7ff0000000000001ULL;
        }
        put_op(stack, *reinterpret_cast<f64_t*>(&result));
    }else{
        put_op(stack, (f64_t)value);
    }
}