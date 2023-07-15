// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Ref_null&){
    // TODO:
}
void RunVisitor::operator()(Instr::Ref_is_null&){
    // TODO:
}
void RunVisitor::operator()(Instr::Ref_func&){
    // TODO:
}