// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include <type_traits>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

static u64_t get_base(Label& label){
  u64_t base = 0;
  Value val = label.values.top();
  label.values.pop();
  if(std::holds_alternative<i32_t>(val)){
    return std::get<i32_t>(val);
  }else{
    return std::get<i64_t>(val);
  }
}

template <typename T, typename I>
  requires (std::is_base_of_v<Instr::MemoryInstr::Base, I>)
static void load_mem(Stack& stack, I& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[addr];
  u64_t base = get_base(label);
  base += instr.offset;
  if((base + sizeof(T)) > mem.data.size()){
    throw Exception::length_too_long();
  }
  label.values.emplace(Value(*(T*)(mem.data.data() + base)));
}

template <typename T, typename S, typename I>
  requires (std::is_base_of_v<Instr::MemoryInstr::Base, I>)
static void load_mem(Stack& stack, I& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[addr];
  u64_t base = get_base(label);
  base += instr.offset;
  if((base + sizeof(S)) > mem.data.size()){
    throw Exception::length_too_long();
  }
  label.values.emplace(Value((T)*(S*)(mem.data.data() + base)));
}

template <typename T, typename I>
  requires (std::is_base_of_v<Instr::MemoryInstr::Base, I>)
static void store_mem(Stack& stack, I& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[addr];
  T val = std::get<T>(label.values.top());
  label.values.pop();
  u64_t base = get_base(label);
  base += instr.offset;
  if((base + sizeof(T)) > mem.data.size()){
    throw Exception::length_too_long();
  }
  *(T*)(mem.data.data() + base) = val;
}

template <typename T, typename S, typename I>
  requires (std::is_base_of_v<Instr::MemoryInstr::Base, I>)
static void store_mem(Stack& stack, I& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[addr];
  T val = std::get<T>(label.values.top());
  label.values.pop();
  u64_t base = get_base(label);
  base += instr.offset;
  if((base + sizeof(S)) > mem.data.size()){
    throw Exception::length_too_long();
  }
  *(S*)(mem.data.data() + base) = (S)val;
}

void RunVisitor::operator()(Instr::I32_load& instr){
  load_mem<i32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load& instr){
  load_mem<i64_t>(stack, instr);
}
void RunVisitor::operator()(Instr::F32_load& instr){
  load_mem<f32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::F64_load& instr){
  load_mem<f64_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_load8_s& instr){
  load_mem<i32_t, i8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_load8_u& instr){
  load_mem<i32_t, u8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_load16_s& instr){
  load_mem<i32_t, i16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_load16_u& instr){
  load_mem<i32_t, u16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load8_s& instr){
  load_mem<i64_t, i8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load8_u& instr){
  load_mem<i64_t, u8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load16_s& instr){
  load_mem<i64_t, i16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load16_u& instr){
  load_mem<i64_t, u16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load32_s& instr){
  load_mem<i64_t, i32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_load32_u& instr){
  load_mem<i64_t, u32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_store& instr){
  store_mem<i32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store& instr){
  store_mem<i64_t>(stack, instr);
}
void RunVisitor::operator()(Instr::F32_store& instr){
  store_mem<f32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::F64_store& instr){
  store_mem<f64_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_store8& instr){
  store_mem<i32_t, i8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_store16& instr){
  store_mem<i32_t, i16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store8& instr){
  store_mem<i64_t, i8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store16& instr){
  store_mem<i64_t, i16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store32& instr){
  store_mem<i64_t, i32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::Memory_size&){
    // TODO:
}
void RunVisitor::operator()(Instr::Memory_grow&){
    // TODO:
}
void RunVisitor::operator()(Instr::Memory_fill&){
    // TODO:
}
void RunVisitor::operator()(Instr::Memory_init&){
    // TODO:
}
void RunVisitor::operator()(Instr::Data_drop&){
    // TODO:
}
void RunVisitor::operator()(Instr::Memory_copy&){
    // TODO:
}