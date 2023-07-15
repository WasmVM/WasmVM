// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Table_get&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_set&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_size&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_grow&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_fill&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_copy&){
    // TODO:
}
void RunVisitor::operator()(Instr::Table_init&){
    // TODO:
}
void RunVisitor::operator()(Instr::Elem_drop&){
    // TODO:
}