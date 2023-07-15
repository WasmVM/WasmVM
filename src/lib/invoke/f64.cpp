// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::F64_const& instr){
    stack.frames.top().labels.top().values.emplace(Value(instr.value));
}
void RunVisitor::operator()(Instr::F64_eq&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_ne&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_lt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_gt&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_le&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_ge&){
    // TODO:
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
    // TODO:
}
void RunVisitor::operator()(Instr::F64_convert_s_i32&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_convert_u_i32&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_convert_s_i64&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_convert_u_i64&){
    // TODO:
}
void RunVisitor::operator()(Instr::F64_promote_f32&){
    // TODO:
}