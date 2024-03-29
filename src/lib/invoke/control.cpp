// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <instances/TableInst.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Unreachable&){
  throw Exception::unreachable();
}

void RunVisitor::operator()(Instr::Nop&){}

void RunVisitor::operator()(Instr::Block& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  WasmFunc& func = std::get<WasmFunc>(stack.store.funcs[frame.funcaddr].body);
  // Get type
  FuncType type;
  if(instr.type){
    type = frame.module.types[instr.type.value()];
  }
  // Pop values
  std::vector<Value> args;
  if(type.params.size() > 0){
    args.resize(type.params.size());
    for(int i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  // Create label
  Label& new_label = frame.labels.emplace();
  new_label.arity = type.results.size();
  new_label.values.insert(args);
  new_label.pc.emplace();
  new_label.pc->current = label.pc->current;
  new_label.pc->continuation = label.pc->current;
  int level = 0;
  while(new_label.pc->continuation < func.body.size()){
    std::visit(overloaded {
      [&](Instr::Block&){
        level += 1;
      },
      [&](Instr::Loop&){
        level += 1;
      },
      [&](Instr::If&){
        level += 1;
      },
      [&](Instr::End&){
        level -= 1;
      },
      [&](auto&){}
    }, func.body[new_label.pc->continuation]);
    if(level >= 0){
      new_label.pc->continuation += 1;
    }else{
      break;
    }
  }
  if(new_label.pc->continuation >= func.body.size() - 1){
    throw Exception::invalid_label();
  }
}

void RunVisitor::operator()(Instr::Loop& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  WasmFunc& func = std::get<WasmFunc>(stack.store.funcs[frame.funcaddr].body);
  // Get type
  FuncType type;
  if(instr.type){
    type = frame.module.types[instr.type.value()];
  }
  // Pop values
  std::vector<Value> args;
  if(type.params.size() > 0){
    args.resize(type.params.size());
    for(int i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  // Create label
  Label& new_label = frame.labels.emplace();
  new_label.arity = type.results.size();
  new_label.values.insert(args);
  new_label.pc.emplace();
  new_label.pc->current = label.pc->current;
  new_label.pc->continuation = label.pc->current;
}

void RunVisitor::operator()(Instr::If& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  WasmFunc& func = std::get<WasmFunc>(stack.store.funcs[frame.funcaddr].body);
  // Get type
  FuncType type;
  if(instr.type){
    type = frame.module.types[instr.type.value()];
  }
  // Pop operant
  bool goElse = (std::get<i32_t>(label.values.top()) == 0);
  label.values.pop();
  // Pop values
  std::vector<Value> args;
  if(type.params.size() > 0){
    args.resize(type.params.size());
    for(int i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  // Create label
  Label& new_label = frame.labels.emplace();
  new_label.arity = type.results.size();
  new_label.values.insert(args);
  new_label.pc.emplace();
  new_label.pc->current = label.pc->current;
  new_label.pc->continuation = label.pc->current;
  int level = 0;
  while(new_label.pc->continuation < func.body.size()){
    std::visit(overloaded {
      [&](Instr::Block&){
        level += 1;
      },
      [&](Instr::Loop&){
        level += 1;
      },
      [&](Instr::If&){
        level += 1;
      },
      [&](Instr::Else&){
        if(goElse && (level == 0)){
          new_label.pc->current = new_label.pc->continuation + 1;
          goElse = false;
        }
      },
      [&](Instr::End&){
        level -= 1;
      },
      [&](auto&){}
    }, func.body[new_label.pc->continuation]);
    if(level >= 0){
      new_label.pc->continuation += 1;
    }else{
      break;
    }
  }
  if(new_label.pc->continuation >= func.body.size() - 1){
    throw Exception::invalid_label();
  }
  // No else
  if(goElse){
    new_label.pc->current = new_label.pc->continuation;
  }
}

void RunVisitor::operator()(Instr::Else&){
Frame& frame = stack.frames.top();
  // Pop values
  std::vector<Value> values = frame.labels.top().values.get();
  Label& label = frame.labels.top();
  // Push values
  if(label.arity > 0){
    label.values.insert(std::vector<Value>(values.end() - label.arity, values.end()));
  }
  // Continuation
  label.pc->current = label.pc->continuation;
}

void RunVisitor::operator()(Instr::End&){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  // Get result
  std::vector<Value> results = label.values.get();
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

void RunVisitor::operator()(Instr::Br& instr){
  Frame& frame = stack.frames.top();
  // Pop values
  std::vector<Value> values = frame.labels.top().values.get();
  // Branch
  for(index_t i = instr.index; i > 0; --i){
    frame.labels.pop();
  }
  Label& label = frame.labels.top();
  // Push values
  if(label.arity > 0){
    label.values.insert(std::vector<Value>(values.end() - label.arity, values.end()));
  }
  // Continuation
  label.pc->current = label.pc->continuation;
}

void RunVisitor::operator()(Instr::Br_if& instr){
  Frame& frame = stack.frames.top();
  bool doBr = (std::get<i32_t>(frame.labels.top().values.top()) != 0);
  frame.labels.top().values.pop();
  if(doBr){
    // Pop values
    std::vector<Value> values = frame.labels.top().values.get();
    // Branch
    for(index_t i = instr.index; i > 0; --i){
      frame.labels.pop();
    }
    Label& label = frame.labels.top();
    // Push values
    if(label.arity > 0){
      label.values.insert(std::vector<Value>(values.end() - label.arity, values.end()));
    }
    // Continuation
    label.pc->current = label.pc->continuation;
  }
}

void RunVisitor::operator()(Instr::Br_table& instr){
  Frame& frame = stack.frames.top();
  i32_t operant = std::get<i32_t>(frame.labels.top().values.top());
  frame.labels.top().values.pop();
  if(operant >= instr.indices.size()){
    operant = instr.indices.size() - 1;
  }
  // Pop values
  std::vector<Value> values = frame.labels.top().values.get();
  // Branch
  for(index_t i = instr.indices[operant]; i > 0; --i){
    frame.labels.pop();
  }
  Label& label = frame.labels.top();
  // Push values
  if(label.arity > 0){
    label.values.insert(std::vector<Value>(values.end() - label.arity, values.end()));
  }
  // Continuation
  label.pc->current = label.pc->continuation;
}

void RunVisitor::operator()(Instr::Return&){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  std::vector<Value> values = label.values.get();
  std::vector<Value> results(values.end() - label.arity, values.end());
  stack.frames.pop();
  if(stack.frames.empty()){
    stack.results = results;
  }else{
    stack.frames.top().labels.top().values.insert(results);
  }
}

void RunVisitor::operator()(Instr::Call& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t funcaddr = frame.module.funcaddrs[instr.index];
  FuncType& type = stack.store.funcs[funcaddr].type;
  std::vector<Value> args;
  if(type.params.size() > 0){
    args.resize(type.params.size());
    for(int i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  stack.invoke(funcaddr, args);
}

void RunVisitor::operator()(Instr::Call_indirect& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  TableInst& table = stack.store.tables[frame.module.tableaddrs[instr.tableidx]];
  i32_t operant = std::get<i32_t>(frame.labels.top().values.top());
  frame.labels.top().values.pop();
  if(operant >= table.elems.size()){
    throw Exception::invalid_elem_index();
  }
  funcref_t funcref = std::get<funcref_t>(table.elems[operant]);
  if(!funcref.has_value()){
    throw Exception::invalid_reference();
  }
  index_t funcaddr = funcref.value();
  FuncType& type = stack.store.funcs[funcaddr].type;
  if(type != frame.module.types[instr.typeidx]){
    throw Exception::function_type_unmatch();
  }
  std::vector<Value> args;
  if(type.params.size() > 0){
    args.resize(type.params.size());
    for(int i = type.params.size() - 1; i >= 0; --i){
      args[i] = label.values.top();
      label.values.pop();
    }
  }
  stack.invoke(funcaddr, args);
}
