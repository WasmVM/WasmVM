// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include <type_traits>
#include <cstring>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

// An address, a length or a count off the operand stack. A 32-bit memory
// takes an i32, which is an *unsigned* quantity: widening it with its sign
// turned -1 into the top of the 64-bit address space instead of 4294967295.
static u64_t get_base(Label& label){
  Value val = label.values.top();
  label.values.pop();
  if(std::holds_alternative<i32_t>(val)){
    return (u64_t)(u32_t)std::get<i32_t>(val);
  }
  return (u64_t)std::get<i64_t>(val);
}

template <typename T, typename I>
  requires (std::is_base_of_v<Instr::MemoryInstr::Base, I>)
static void load_mem(Stack& stack, I& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[addr];
  u64_t base = get_base(label);
  base = offset_from(base, instr.offset);
  if(!fits_within(base, sizeof(T), mem.data.size())){
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
  base = offset_from(base, instr.offset);
  if(!fits_within(base, sizeof(S), mem.data.size())){
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
  base = offset_from(base, instr.offset);
  if(!fits_within(base, sizeof(T), mem.data.size())){
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
  base = offset_from(base, instr.offset);
  if(!fits_within(base, sizeof(S), mem.data.size())){
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
  store_mem<i32_t, u8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I32_store16& instr){
  store_mem<i32_t, u16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store8& instr){
  store_mem<i64_t, u8_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store16& instr){
  store_mem<i64_t, u16_t>(stack, instr);
}
void RunVisitor::operator()(Instr::I64_store32& instr){
  store_mem<i64_t, u32_t>(stack, instr);
}
void RunVisitor::operator()(Instr::Memory_size& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.index];
  MemInst& mem = stack.store.mems[addr];
  u64_t pages = mem.data.size() / page_size;
  if(mem.type.is64)
    label.values.emplace<i64_t>((i64_t)pages);
  else
    label.values.emplace<i32_t>((i32_t)pages);
}
void RunVisitor::operator()(Instr::Memory_grow& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.index];
  MemInst& mem = stack.store.mems[addr];
  u64_t old_pages = mem.data.size() / page_size;
  u64_t val = get_base(label);
  // get_base no longer sign-extends, but a 64-bit memory can still be asked
  // for a count that overflows the sum.
  if(val > ((u64_t)-1 - old_pages)){
    if(mem.type.is64)
      label.values.emplace<i64_t>(i64_t(-1));
    else
      label.values.emplace<i32_t>(i32_t(-1));
    return;
  }
  u64_t new_pages = old_pages + val;
  // A 32-bit memory addresses at most 2^32 bytes, which is 2^16 pages; the
  // 2^48 bound belongs to a 64-bit one. Applying the wide bound to both let a
  // 32-bit memory be asked for 2^32 pages and try to allocate 256 TiB, which
  // came out of the allocator as a std::bad_alloc rather than as -1.
  const u64_t page_limit = mem.type.is64 ? (1ULL << 48ULL) : (1ULL << 16ULL);
  if((new_pages > page_limit) || (mem.type.max && (new_pages > mem.type.max.value()))) {
    if(mem.type.is64)
      label.values.emplace<i64_t>(i64_t(-1));
    else
      label.values.emplace<i32_t>(i32_t(-1));
    return;
  }
  mem.data.resize(new_pages * page_size, (byte_t)0x00);
  mem.type.min = new_pages;
  // Return the OLD page count (per spec)
  if(mem.type.is64)
    label.values.emplace<i64_t>((i64_t)old_pages);
  else
    label.values.emplace<i32_t>((i32_t)old_pages);
}
void RunVisitor::operator()(Instr::Memory_fill& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t addr = frame.module.memaddrs[instr.index];
  MemInst& mem = stack.store.mems[addr];
  u64_t count = get_base(label);
  byte_t val = (byte_t)std::get<i32_t>(label.values.top());
  label.values.pop();
  u64_t index = get_base(label);
  if(!fits_within(index, count, mem.data.size())){
    throw Exception::length_too_long();
  }
  std::fill_n(mem.data.begin() + index, count, val);
}
void RunVisitor::operator()(Instr::Memory_copy& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t src_addr = frame.module.memaddrs[instr.srcidx];
  MemInst& src_mem = stack.store.mems[src_addr];
  index_t dst_addr = frame.module.memaddrs[instr.dstidx];
  MemInst& dst_mem = stack.store.mems[dst_addr];
  u64_t count = get_base(label);
  u64_t src_idx = get_base(label);
  u64_t dst_idx = get_base(label);
  if(!fits_within(src_idx, count, src_mem.data.size())
     || !fits_within(dst_idx, count, dst_mem.data.size())){
    throw Exception::length_too_long();
  }
  // The spec copies backwards when the destination lies past the source, so
  // an overlapping copy within one memory must behave like memmove.
  if(count > 0){
    std::memmove(dst_mem.data.data() + dst_idx, src_mem.data.data() + src_idx, count);
  }
}
void RunVisitor::operator()(Instr::Memory_init& instr){
  Frame& frame = stack.frames.top();
  Label& label = frame.labels.top();
  index_t mem_addr = frame.module.memaddrs[instr.memidx];
  MemInst& mem = stack.store.mems[mem_addr];
  index_t data_addr = frame.module.dataaddrs[instr.dataidx];
  DataInst& data = stack.store.datas[data_addr];
  u64_t count = get_base(label);
  u64_t src_idx = get_base(label);
  u64_t dst_idx = get_base(label);
  if(!fits_within(src_idx, count, data.size())
     || !fits_within(dst_idx, count, mem.data.size())){
    throw Exception::length_too_long();
  }
  std::copy_n(data.begin() + src_idx, count, mem.data.begin() + dst_idx);
}
void RunVisitor::operator()(Instr::Data_drop& instr){
  Frame& frame = stack.frames.top();
  index_t addr = frame.module.dataaddrs[instr.index];
  DataInst& data = stack.store.datas[addr];
  data.clear();
}