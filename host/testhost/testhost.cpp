// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>
#include <instances/ModuleInst.hpp>
#include <instances/Stack.hpp>
#include <map>
#include <filesystem>
#include <vector>
#include <iostream>
#include <cmath>

using namespace WasmVM;

static std::vector<Value> print_i32(Stack& stack){
  Frame frame = stack.frames.top();
  std::cout << "print_i32 " << std::get<i32_t>(frame.locals[0]) << std::endl;
  return {};
}

static std::vector<Value> print_i64(Stack& stack){
  Frame frame = stack.frames.top();
  std::cout << "print_i64 " << std::get<i64_t>(frame.locals[0]) << std::endl;
  return {};
}

static std::vector<Value> print_f32(Stack& stack){
  Frame frame = stack.frames.top();
  std::cout << "print_f32 ";
  f32_t value = std::get<f32_t>(frame.locals[0]);
  if(std::isnan(value)){
    u32_t uvalue = *(u32_t*)(&value);
    if(uvalue & 0x80000000UL){
      std::cout << "-";
    }
    std::cout << "nan";
    u32_t mantissa = uvalue & 0x7fffffUL;
    if(mantissa != 0x400000UL){
      auto old_fmt = std::cout.flags();
      std::cout << ":0x" << std::hex << mantissa;
      std::cout.flags(old_fmt);
    }
  }else{
    std::cout << value;
  }
  std::cout << std::endl;
  return {};
}

static std::vector<Value> print_f64(Stack& stack){
  Frame frame = stack.frames.top();
  std::cout << "print_f64 ";
  f64_t value = std::get<f64_t>(frame.locals[0]);
  if(std::isnan(value)){
    u64_t uvalue = *reinterpret_cast<u64_t*>(&value);
    if(uvalue & 0x8000000000000000ULL){
      std::cout << "-";
    }
    std::cout << "nan";
    u64_t mantissa = uvalue & 0xfffffffffffffULL;
    if(mantissa != 0x8000000000000ULL){
      auto old_fmt = std::cout.flags();
      std::cout << ":0x" << std::hex << mantissa;
      std::cout.flags(old_fmt);
    }
  }else{
    std::cout << value;
  }
  std::cout << std::endl;
  return {};
}

static std::vector<Value> print_funcref(Stack& stack){
  Frame frame = stack.frames.top();
  auto funcref = std::get<funcref_t>(frame.locals[0]);
  std::cout << "print_funcref ";
  if(funcref){
    std::cout << funcref.value();
  }else{
    std::cout << "null";
  }
  std::cout << std::endl;
  return {};
}

void testhost_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store){
  if(moduleinsts.contains("testhost")){
    return;
  }
  moduleinsts.emplace("testhost", ModuleInst());
  ModuleInst& moduleinst = moduleinsts["testhost"];

  moduleinst.funcaddrs.emplace_back(store.funcs.size());
  FuncInst& func_i32 = store.funcs.emplace_back(moduleinst);
  func_i32.module = moduleinst;
  func_i32.type.params.emplace_back(ValueType::i32);
  func_i32.body.emplace<hostfunc_t>(print_i32);

  moduleinst.funcaddrs.emplace_back(store.funcs.size());
  FuncInst& func_i64 = store.funcs.emplace_back(moduleinst);
  func_i64.module = moduleinst;
  func_i64.type.params.emplace_back(ValueType::i64);
  func_i64.body.emplace<hostfunc_t>(print_i64);

  moduleinst.funcaddrs.emplace_back(store.funcs.size());
  FuncInst& func_f32 = store.funcs.emplace_back(moduleinst);
  func_f32.module = moduleinst;
  func_f32.type.params.emplace_back(ValueType::f32);
  func_f32.body.emplace<hostfunc_t>(print_f32);

  moduleinst.funcaddrs.emplace_back(store.funcs.size());
  FuncInst& func_f64 = store.funcs.emplace_back(moduleinst);
  func_f64.module = moduleinst;
  func_f64.type.params.emplace_back(ValueType::f64);
  func_f64.body.emplace<hostfunc_t>(print_f64);

  moduleinst.funcaddrs.emplace_back(store.funcs.size());
  FuncInst& func_funcref = store.funcs.emplace_back(moduleinst);
  func_funcref.module = moduleinst;
  func_funcref.type.params.emplace_back(ValueType::funcref);
  func_funcref.body.emplace<hostfunc_t>(print_funcref);

  ExportInst& export_i32 = moduleinst.exports.emplace_back();
  export_i32.name = "print_i32";
  export_i32.value.type = ExternVal::ExternType::Func;
  export_i32.value.addr = moduleinst.funcaddrs[0];

  ExportInst& export_i64 = moduleinst.exports.emplace_back();
  export_i64.name = "print_i64";
  export_i64.value.type = ExternVal::ExternType::Func;
  export_i64.value.addr = moduleinst.funcaddrs[1];

  ExportInst& export_f32 = moduleinst.exports.emplace_back();
  export_f32.name = "print_f32";
  export_f32.value.type = ExternVal::ExternType::Func;
  export_f32.value.addr = moduleinst.funcaddrs[2];

  ExportInst& export_f64 = moduleinst.exports.emplace_back();
  export_f64.name = "print_f64";
  export_f64.value.type = ExternVal::ExternType::Func;
  export_f64.value.addr = moduleinst.funcaddrs[3];

  ExportInst& export_funcref = moduleinst.exports.emplace_back();
  export_funcref.name = "print_funcref";
  export_funcref.value.type = ExternVal::ExternType::Func;
  export_funcref.value.addr = moduleinst.funcaddrs[4];
}