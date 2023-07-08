// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

#include <iostream>

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