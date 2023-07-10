// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::I32_const& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  label.values.push(Value(instr.value));
}