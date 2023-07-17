// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Local_get& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  label.values.emplace(frame.locals[instr.index]);
}
void RunVisitor::operator()(Instr::Local_set& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  frame.locals[instr.index] = label.values.top();
  label.values.pop();
}
void RunVisitor::operator()(Instr::Local_tee& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  frame.locals[instr.index] = label.values.top();
}
void RunVisitor::operator()(Instr::Global_get& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t globaladdr = frame.module.globaladdrs[instr.index];
  label.values.emplace(stack.store.globals[globaladdr].value);
}
void RunVisitor::operator()(Instr::Global_set& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t globaladdr = frame.module.globaladdrs[instr.index];
  stack.store.globals[globaladdr].value = label.values.top();
  stack.store.globals[globaladdr].type = (ValueType)label.values.top().index();
  label.values.pop();
}