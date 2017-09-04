/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use
this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations
under the License.
*/

#include <VariableInstr.h>

void Instruction::get_local(std::uint32_t index, Stack& coreStack) {
  // Check local
  if (index >= coreStack.curFrame->locals.size()) {
    throw Exception("[get_local] No such local variable in this frame.",
                    coreStack);
  }
  // Push value
  coreStack.push(coreStack.curFrame->locals.at(index));
}
void Instruction::set_local(std::uint32_t index, Stack& coreStack) {
  // Check local
  if (index >= coreStack.curFrame->locals.size()) {
    throw Exception("[set_local] No such local variable in this frame.",
                    coreStack);
  }
  // Check value
  if (coreStack.valueNum < 1) {
    throw Exception("[set_local] No value in the stack.", coreStack);
  }
  // Pop value
  Value* val = (Value*)coreStack.pop().data;
  // Replace
  coreStack.curFrame->locals.at(index) = *val;
  // Clean
  delete val;
}
void Instruction::tee_local(std::uint32_t index, Stack& coreStack) {
  // Check local
  if (index >= coreStack.curFrame->locals.size()) {
    throw Exception("[tee_local] No such local variable in this frame.",
                    coreStack);
  }
  // Check value
  if (coreStack.valueNum < 1) {
    throw Exception("[tee_local] No value in the stack.", coreStack);
  }
  // Get value
  Value* val = (Value*)coreStack.top().data;
  // Replace
  coreStack.curFrame->locals.at(index) = *val;
}
void Instruction::get_global(std::uint32_t index,
                             Store& store,
                             Stack& coreStack) {
  // Check global index
  if (index >= coreStack.curFrame->moduleInst->globaladdrs.size()) {
    throw Exception("[get_global] No such global address in this frame.",
                    coreStack);
  }
  std::uint32_t globalAddr =
      coreStack.curFrame->moduleInst->globaladdrs.at(index);
  // Check global
  if (globalAddr >= store.globals.size()) {
    throw Exception("[get_global] No such global variable in the store.",
                    coreStack);
  }
  // Push value
  coreStack.push(store.globals.at(globalAddr)->val);
}
void Instruction::set_global(std::uint32_t index,
                             Store& store,
                             Stack& coreStack) {
  // Check global index
  if (index >= coreStack.curFrame->moduleInst->globaladdrs.size()) {
    throw Exception("[set_global] No such global address in this frame.",
                    coreStack);
  }
  std::uint32_t globalAddr =
      coreStack.curFrame->moduleInst->globaladdrs.at(index);
  // Check global
  if (globalAddr >= store.globals.size()) {
    throw Exception("[set_global] No such global variable in the store.",
                    coreStack);
  }
  // Pop value
  Value* val = (Value*)coreStack.pop().data;
  // Replace
  store.globals.at(globalAddr)->val = *val;
  // Clean
  delete val;
}
