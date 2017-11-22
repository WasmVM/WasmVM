/*
Copyright (c) 2017 Luis Hsu, Shu-Min Cyu

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

#include <Syscall.h>

std::map<std::string, ModuleInst*>* Syscall::moduleInsts = nullptr;
void Syscall::initSyscall(std::map<std::string, ModuleInst*>* moduleInsts) {
  Syscall::moduleInsts = moduleInsts;
}

void Syscall::handle(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 1) {
    throw Exception("[unreachable] No value in the stack.", coreStack);
  }
  // Pop operand
  Value* operand = (Value*)coreStack.pop().data;
  if (operand->type != i32) {
    throw Exception("[unreachable] Operand type is not i32.", coreStack);
  }
  // Call
  switch (operand->data.i32) {
    case SYS_stdin:
      sys_stdin(store, coreStack);
      break;
    case SYS_stdout:
      sys_stdout(store, coreStack);
      break;
    case SYS_stderr:
      sys_stderr(store, coreStack);
      break;
    default:
      throw Exception("[unreachable] Non-available operand.", coreStack);
      break;
  }
  // Clean
  delete operand;
}

void Syscall::sys_stdin(Store& store, Stack& coreStack){
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_stdin] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  if (len->type != i32 || bufAddr->type != i32) {
    throw Exception("[syscall][sys_stdin] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_stdin] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_stdin] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* bufPtr = memoryData += bufAddr->data.i32;
  // Sys_read
  std::int32_t ret =
      read(STDIN_FILENO, bufPtr, len->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_stdin] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete len;
  delete bufAddr;
}
void Syscall::sys_stdout(Store& store, Stack& coreStack){
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_stdout] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  if (len->type != i32 || bufAddr->type != i32) {
    throw Exception("[syscall][sys_stdout] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_stdout] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_stdout] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* bufPtr = memoryData += bufAddr->data.i32;
  // Sys_write
  std::int32_t ret =
      write(STDOUT_FILENO, bufPtr, len->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_stdout] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete len;
  delete bufAddr;
}
void Syscall::sys_stderr(Store& store, Stack& coreStack){
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_stderr] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  if (len->type != i32 || bufAddr->type != i32) {
    throw Exception("[syscall][sys_stderr] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_stderr] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_stderr] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* bufPtr = memoryData += bufAddr->data.i32;
  // Sys_write
  std::int32_t ret =
      write(STDERR_FILENO, bufPtr, len->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_stderr] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete len;
  delete bufAddr;
}
