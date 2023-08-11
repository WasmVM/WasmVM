// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"
#include <algorithm>

using namespace WasmVM;

static Ref get_ref(Value& val){
  if(std::holds_alternative<funcref_t>(val)){
    return std::get<funcref_t>(val);
  }else if(std::holds_alternative<externref_t>(val)){
    return std::get<externref_t>(val);
  }
  throw Exception::invalid_value();
}

void RunVisitor::operator()(Instr::Table_get& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  i32_t idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  std::visit(overloaded {
    [&](auto& ref){
      label.values.emplace(ref);
    }
  }, stack.store.tables[addr].elems[idx]);
}
void RunVisitor::operator()(Instr::Table_set& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  Ref ref = get_ref(label.values.top());
  label.values.pop();
  i32_t idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  stack.store.tables[addr].elems[idx] = ref;
}
void RunVisitor::operator()(Instr::Table_size& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  label.values.emplace<i32_t>(stack.store.tables[addr].elems.size());
}
void RunVisitor::operator()(Instr::Table_grow& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  TableInst& table = stack.store.tables[addr];
  i32_t count = std::get<i32_t>(label.values.top());
  label.values.pop();
  Ref ref = get_ref(label.values.top());
  label.values.pop();
  size_t len = count + table.elems.size();
  if((len >= (u32_t)-1) || (table.type.limits.max && (len > table.type.limits.max.value()))){
    label.values.emplace<i32_t>(-1);
    return;
  }
  table.elems.resize(len, ref);
  table.type.limits.min = len;
  label.values.emplace<i32_t>(len);
}
void RunVisitor::operator()(Instr::Table_fill& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  TableInst& table = stack.store.tables[addr];
  i32_t count = std::get<i32_t>(label.values.top());
  label.values.pop();
  Ref ref = get_ref(label.values.top());
  label.values.pop();
  i32_t index = std::get<i32_t>(label.values.top());
  label.values.pop();
  if((index + count) > table.elems.size()){
    throw Exception::length_too_long();
  }
  std::fill_n(table.elems.begin() + index, count, ref);
}
void RunVisitor::operator()(Instr::Table_copy& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t src_addr = frame.module.tableaddrs[instr.srcidx];
  TableInst& src_table = stack.store.tables[src_addr];
  index_t dst_addr = frame.module.tableaddrs[instr.dstidx];
  TableInst& dst_table = stack.store.tables[dst_addr];
  i32_t count = std::get<i32_t>(label.values.top());
  label.values.pop();
  i32_t src_idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  i32_t dst_idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  if(((src_idx + count) > src_table.elems.size()) || ((dst_idx + count) > dst_table.elems.size())){
    throw Exception::length_too_long();
  }
  std::copy_n(src_table.elems.begin() + src_idx, count, dst_table.elems.begin() + dst_idx);
}
void RunVisitor::operator()(Instr::Table_init& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t table_addr = frame.module.tableaddrs[instr.tableidx];
  TableInst& table = stack.store.tables[table_addr];
  index_t elem_addr = frame.module.elemaddrs[instr.elemidx];
  ElemInst& elem = stack.store.elems[elem_addr];
  i32_t count = std::get<i32_t>(label.values.top());
  label.values.pop();
  i32_t src_idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  i32_t dst_idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  if(((src_idx + count) > elem.elem.size()) || ((dst_idx + count) > table.elems.size())){
    throw Exception::length_too_long();
  }
  std::copy_n(elem.elem.begin() + src_idx, count, table.elems.begin() + dst_idx);
}
void RunVisitor::operator()(Instr::Elem_drop& instr){
  Frame& frame = stack.frames.top();
  index_t addr = frame.module.elemaddrs[instr.index];
  ElemInst& elem = stack.store.elems[addr];
  elem.elem.clear();
}