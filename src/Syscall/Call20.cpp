/*
Copyright 2017 Luis Hsu, Kevin Cyu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/
#include <Call20.h>

#include <Call20.h>

void Call::sysRead(Store &store, Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 3){
        throw Exception("[syscall][sys_read] No enough value in the stack.", coreStack);
    }
    /* Pop operand - (int) fd, (void *)buf,(size_t) count
    */
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_read] Operand type is not i32.", coreStack);
    }
    // Pop out buf addr
    Value *bufAddr = (Value *)coreStack.pop().data;
    if(bufAddr->type != i32){
        throw Exception("[syscall][sys_read] Operand type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_read] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr >= store.mems.size()){
        throw Exception("[syscall][sys_read] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *bufPtr = memoryData += bufAddr->data.i32;
    // Pop out count
    Value *count = (Value *)coreStack.pop().data;
    if(count->type != i32){
        throw Exception("[syscall][sys_read] Operand type is not i32.", coreStack);
    }
    // Sys_Read 
    read((std::int32_t)fd->data.i32,(void *)bufPtr,(size_t)count->data.i32);
    // Clean
    delete fd;
    delete bufAddr;
    delete count;
}

void Call::sysWrite(Store &store, Stack &coreStack){
	// Check value count 
    if(coreStack.valueNum < 3){
        throw Exception("[syscall][sys_write] No enough value in the stack.", coreStack);
    }
    /* Pop operand - (int) fd, (void *)buf,(size_t) count
    */
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_write] Operand type is not i32.", coreStack);
    }
    // Pop out buf addr
    Value *bufAddr = (Value *)coreStack.pop().data;
    if(bufAddr->type != i32){
        throw Exception("[syscall][sys_write] Operand type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_write] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr >= store.mems.size()){
        throw Exception("[syscall][sys_write] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *bufPtr = memoryData += bufAddr->data.i32;
    // Pop out count
    Value *count = (Value *)coreStack.pop().data;
    if(count->type != i32){
        throw Exception("[syscall][sys_write] Operand type is not i32.", coreStack);
    }
    // Sys_Read 
    write((std::int32_t)fd->data.i32,(void *)bufPtr,(size_t)count->data.i32);
    // Clean
    delete fd;
    delete bufAddr;
    delete count;
}

void Call::sysOpen(Store &store, Stack &coreStack){
	// Check value count 
	if(coreStack.valueNum < 3){
        throw Exception("[syscall][sys_open] No enough value in the stack.", coreStack);   
    }
    /* Pop operand - (char*)pathname , (int)flags
    Using `int open(const char *pathname,int flags);` as implementation
    */
    // Pop out address of Pathname
    Value *pathnameAddr = (Value *)coreStack.pop().data;
    if(pathnameAddr->type != i32){
        throw Exception("[syscall][sys_open] Operand type is not i32.", coreStack);
    }
    // Find out the body of pathname by addr
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_open] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr >= store.mems.size()){
        throw Exception("[syscall][sys_open] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *pathnamePtr = memoryData += pathnameAddr->data.i32;
    // Pop out flags 
    Value *flags = (Value *)coreStack.pop().data;
    if(flags->type != i32){
        throw Exception("[syscall][sys_open] Operand type is not i32.", coreStack);
    }
    // Sys_open , keep the returning file descriptor
    std::int32_t ret_fd = open(pathnamePtr, flags->data.i32);
    // Push back
    coreStack.push(ret_fd);
    // Clean 
    delete pathnameAddr;
    delete flags; 
}

void Call::sysClose(Stack &coreStack){
    // Check value count 
	if(coreStack.valueNum < 1){
        throw Exception("[syscall][sys_close] No enough value in the stack.", coreStack);   
    }
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_close] Operand type is not i32.", coreStack);
    }
    // Sys_Close
    if(close((int)fd->data.i32) == -1){
        throw Exception("[syscall][sys_close] System call failed.", coreStack);
    }
    delete fd;
}