// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::I32_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::I32_eqz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_eq&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_ne&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_lt_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_lt_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_gt_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_gt_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_le_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_le_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_ge_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_ge_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_wrap_i64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_s_f32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_u_f32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_s_f64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_u_f64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_clz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_ctz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_popcnt&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_add&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_sub&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_mul&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_div_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_div_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_rem_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_rem_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_and&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_or&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_xor&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_shl&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_shr_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_shr_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_rotl&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_rotr&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_reinterpret_f32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_extend8_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_extend16_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f32_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f32_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f64_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I32_trunc_sat_f64_u&){
    // TODO:
}