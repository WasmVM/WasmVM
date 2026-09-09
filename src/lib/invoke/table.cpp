// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"
#include <algorithm>

using namespace WasmVM;

// A table slot holds a Value now, so the only thing to check is that what
// goes in is a reference at all.
static Value get_ref(Value& val){
  if(std::holds_alternative<i32_t>(val) || std::holds_alternative<i64_t>(val)
     || std::holds_alternative<f32_t>(val) || std::holds_alternative<f64_t>(val)){
    throw Exception::invalid_value();
  }
  return val;
}

void RunVisitor::operator()(Instr::Table_get& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  i32_t idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  TableInst& table = stack.store.tables[addr];
  // Compare as unsigned so a negative i32 index also traps.
  if((u32_t)idx >= table.elems.size()){
    throw Exception::length_too_long();
  }
  std::visit(overloaded {
    [&](auto& ref){
      label.values.emplace(ref);
    }
  }, table.elems[idx]);
}
void RunVisitor::operator()(Instr::Table_set& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  Value ref = get_ref(label.values.top());
  label.values.pop();
  i32_t idx = std::get<i32_t>(label.values.top());
  label.values.pop();
  TableInst& table = stack.store.tables[addr];
  // Compare as unsigned so a negative i32 index also traps.
  if((u32_t)idx >= table.elems.size()){
    throw Exception::length_too_long();
  }
  table.elems[idx] = ref;
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
  Value ref = get_ref(label.values.top());
  label.values.pop();
  size_t len = count + table.elems.size();
  if((len >= (u32_t)-1) || (table.type.limits.max && (len > table.type.limits.max.value()))){
    label.values.emplace<i32_t>(-1);
    return;
  }
  const size_t old_len = table.elems.size();
  table.elems.resize(len, ref);
  table.type.limits.min = len;
  // Return the OLD length (per spec), as memory.grow does.
  label.values.emplace<i32_t>((i32_t)(u32_t)old_len);
}
void RunVisitor::operator()(Instr::Table_fill& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.tableaddrs[instr.index];
  TableInst& table = stack.store.tables[addr];
  i32_t count = std::get<i32_t>(label.values.top());
  label.values.pop();
  Value ref = get_ref(label.values.top());
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