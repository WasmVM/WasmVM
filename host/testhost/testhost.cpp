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

using namespace WasmVM;

static std::vector<Value> print_i32(Stack& stack){
  Frame frame = stack.frames.top();
  std::cout << "print_i32 " << std::get<i32_t>(frame.locals[0]) << std::endl;
  return {};
}

void testhost_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store){
  if(moduleinsts.contains("testhost")){
    return;
  }
  moduleinsts.emplace("testhost", ModuleInst());
  ModuleInst& moduleinst = moduleinsts["testhost"];
  index_t funcaddr = store.funcs.size();
  moduleinst.funcaddrs.emplace_back(funcaddr);

  FuncInst& funcinst = store.funcs.emplace_back(moduleinst);
  funcinst.module = moduleinst;
  funcinst.type.params.emplace_back(ValueType::i32);
  funcinst.body.emplace<hostfunc_t>(print_i32);

  ExportInst& exportinst = moduleinst.exports.emplace_back();
  exportinst.name = "print_i32";
  exportinst.value.type = ExternVal::Func;
  exportinst.value.addr = funcaddr;
}