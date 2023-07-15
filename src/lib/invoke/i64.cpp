// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::I64_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::I64_eqz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_eq&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_ne&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_lt_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_lt_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_gt_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_gt_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_le_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_le_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_ge_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_ge_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_clz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_ctz&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_popcnt&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_add&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_sub&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_mul&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_div_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_div_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_rem_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_rem_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_and&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_or&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_xor&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_shl&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_shr_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_shr_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_rotl&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_rotr&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_extend_s_i32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_extend_u_i32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_s_f32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_u_f32&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_s_f64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_u_f64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_reinterpret_f64&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_extend8_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_extend16_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_extend32_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f32_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f32_u&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f64_s&){
    // TODO:
}
void RunVisitor::operator()(Instr::I64_trunc_sat_f64_u&){
    // TODO:
}