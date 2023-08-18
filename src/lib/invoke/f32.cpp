// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"
#include "numeric.hpp"
#include <cmath>
#include <cfenv>
#include <limits>

using namespace WasmVM;

void RunVisitor::operator()(Instr::F32_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::F32_eq&){
    auto ops = get_ops<f32_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)0);
    }else if(std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO){
        put_op(stack, (i32_t)1);
    }else{
        put_op(stack, (i32_t)(ops.first == ops.second));
    }
}
void RunVisitor::operator()(Instr::F32_ne&){
    auto ops = get_ops<f32_t>(stack);
    if(std::isnan(ops.first) || std::isnan(ops.second)){
        put_op(stack, (i32_t)1);
    }else if(std::fpclassify(ops.first) == FP_ZERO && std::fpclassify(ops.second) == FP_ZERO){
        put_op(stack, (i32_t)0);
    }else{
        put_op(stack, (i32_t)(ops.first != ops.second));
    }
}
void RunVisitor::operator()(Instr::F32_lt&){
    auto ops = get_ops<f32_t>(stack);
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
void RunVisitor::operator()(Instr::F32_gt&){
    auto ops = get_ops<f32_t>(stack);
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
void RunVisitor::operator()(Instr::F32_le&){
    auto ops = get_ops<f32_t>(stack);
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
void RunVisitor::operator()(Instr::F32_ge&){
    auto ops = get_ops<f32_t>(stack);
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
void RunVisitor::operator()(Instr::F32_abs&){
    put_op(stack, std::fabs(get_op<f32_t>(stack)));
}
void RunVisitor::operator()(Instr::F32_neg&){
    put_op(stack, -get_op<f32_t>(stack));
}
void RunVisitor::operator()(Instr::F32_ceil&){
    put_op(stack, std::ceil(get_op<f32_t>(stack)));
}
void RunVisitor::operator()(Instr::F32_floor&){
    put_op(stack, std::floor(get_op<f32_t>(stack)));
}
void RunVisitor::operator()(Instr::F32_trunc&){
    put_op(stack, std::trunc(get_op<f32_t>(stack)));
}
void RunVisitor::operator()(Instr::F32_nearest&){
    auto old_env = std::fegetround();
    std::fesetround(FE_TONEAREST);
    put_op(stack, std::nearbyint(get_op<f32_t>(stack)));
    std::fesetround(old_env);
}
void RunVisitor::operator()(Instr::F32_sqrt&){
    put_op(stack, std::sqrt(get_op<f32_t>(stack)));
}
void RunVisitor::operator()(Instr::F32_add&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.first : ops.second);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_INFINITE && classifies.second == FP_INFINITE){
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? ops.first : std::nanf(""));
        }else{
            put_op(stack, (classifies.first == FP_INFINITE) ? ops.first : ops.second);
        }
    }else if(classifies.first == FP_ZERO || classifies.second == FP_ZERO){
        if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? ops.first : 0.0f);
        }else{
            put_op(stack, (classifies.first == FP_ZERO) ? ops.second : ops.first);
        }
    }else{
        put_op(stack, ops.first + ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_sub&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.first : ops.second);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_INFINITE && classifies.second == FP_INFINITE){
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? std::nanf("") : ops.first);
        }else{
            put_op(stack, (classifies.first == FP_INFINITE) ? ops.first : -ops.second);
        }
    }else if(classifies.first == FP_ZERO || classifies.second == FP_ZERO){
        if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? 0.0f : ops.first);
        }else{
            put_op(stack, (classifies.first == FP_ZERO) ? -ops.second : ops.first);
        }
    }else{
        put_op(stack, ops.first - ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_mul&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.first : ops.second);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_ZERO || classifies.second == FP_ZERO){
            put_op(stack, std::nanf(""));
        }else{
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? std::numeric_limits<f32_t>::infinity() : -std::numeric_limits<f32_t>::infinity());
        }
    }else if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
        put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? +0.0f : -0.0f);
    }else{
        put_op(stack, ops.first * ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_div&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.first : ops.second);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_INFINITE && classifies.second == FP_INFINITE){
            put_op(stack, std::nanf(""));
        }else{
            f32_t result = (classifies.first == FP_INFINITE) ? std::numeric_limits<f32_t>::infinity() : 0.0f;
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? result : -result);
        }
    }else if(classifies.first == FP_ZERO || classifies.second == FP_ZERO){
        if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
            put_op(stack, std::nanf(""));
        }else{
            f32_t result = (classifies.first == FP_ZERO) ? 0.0f : std::numeric_limits<f32_t>::infinity();
            put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? result : -result);
        }
    }else{
        put_op(stack, ops.first / ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_min&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.second : ops.first);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_INFINITE){
            put_op(stack, std::signbit(ops.first) ? ops.first : ops.second);
        }else{
            put_op(stack, std::signbit(ops.second) ? ops.second : ops.first);
        }
    }else if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
        put_op(stack, (std::signbit(ops.first) != std::signbit(ops.second)) ? -0.0f : ops.first);
    }else{
        put_op(stack, (ops.first <= ops.second) ? ops.first : ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_max&){
    auto ops = get_ops<f32_t>(stack);
    std::pair<int, int> classifies {std::fpclassify(ops.first), std::fpclassify(ops.second)};
    if(classifies.first == FP_NAN || classifies.second == FP_NAN){
        put_op(stack, (classifies.first == FP_NAN) ? ops.second : ops.first);
    }else if(classifies.first == FP_INFINITE || classifies.second == FP_INFINITE){
        if(classifies.first == FP_INFINITE){
            put_op(stack, std::signbit(ops.first) ? ops.second : ops.first);
        }else{
            put_op(stack, std::signbit(ops.second) ? ops.first : ops.second);
        }
    }else if(classifies.first == FP_ZERO && classifies.second == FP_ZERO){
        put_op(stack, (std::signbit(ops.first) != std::signbit(ops.second)) ? 0.0f : ops.first);
    }else{
        put_op(stack, (ops.first >= ops.second) ? ops.first : ops.second);
    }
}
void RunVisitor::operator()(Instr::F32_copysign&){
    auto ops = get_ops<f32_t>(stack);
    put_op(stack, (std::signbit(ops.first) == std::signbit(ops.second)) ? ops.first : -ops.first);
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
        f32_t result = std::nanf("");
        if((*reinterpret_cast<u64_t*>(&value) & 0x7fffffffffffffffULL) != 0x7ff8000000000000ULL){
            // non-canonical 
            result = std::nanf("1");
        }
        put_op(stack, std::signbit(value) ? -result : result);
    }else{
        put_op(stack, (f32_t)value);
    }
}