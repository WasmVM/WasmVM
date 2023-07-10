// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Nop&){}

void RunVisitor::operator()(Instr::Unreachable&){
  throw Exception::unreachable();
}

void RunVisitor::operator()(Instr::End&){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  // Get result
  std::vector<Value> results = label.values.get();
  if(results.size() != label.arity){
    throw Exception::invalid_results();
  }
  // exit label
  size_t current = label.pc->current;
  frame.labels.pop();
  if(frame.labels.empty()){
    // function return
    stack.frames.pop();
    if(stack.frames.empty()){
      stack.results = results;
    }else{
      stack.frames.top().labels.top().values.insert(results);
    }
  }else{
    // end block
    frame.labels.top().values.insert(results);
    if(frame.labels.top().pc){
      frame.labels.top().pc->current = current;
    }
  }
}

void RunVisitor::operator()(Instr::Call& instr){
  Frame& frame = stack.frames.top();
  if(frame.labels.empty()){
    throw Exception::invalid_label();
  }
  Label& label = frame.labels.top();
  index_t funcaddr = frame.module.funcaddrs[instr.index];
  if(funcaddr >= stack.store.funcs.size()){
    throw Exception::func_not_exist();
  }
  FuncType& type = stack.store.funcs[funcaddr].type;
  std::vector<Value> args;
  if(type.params.size() > 0){
    for(size_t i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  stack.invoke(funcaddr, args);
}
