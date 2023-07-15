// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Local_get&){
    // TODO:
}
void RunVisitor::operator()(Instr::Local_set&){
    // TODO:
}
void RunVisitor::operator()(Instr::Local_tee&){
    // TODO:
}
void RunVisitor::operator()(Instr::Global_get&){
    // TODO:
}
void RunVisitor::operator()(Instr::Global_set&){
    // TODO:
}