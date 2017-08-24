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

void Call::sysRead(Store &store, Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 3){
        throw Exception("[syscall][sys_read] No enough value in the stack.", coreStack);
    }
    /* Pop operand - (int) fd, (void *)buf,(size_t) count
    */
    // Pop out count
    Value *count = (Value *)coreStack.pop().data;
    if(count->type != i32){
        throw Exception("[syscall][sys_read] count type is not i32.", coreStack);
    }
    // Pop out buf addr
    Value *bufAddr = (Value *)coreStack.pop().data;
    if(bufAddr->type != i32){
        throw Exception("[syscall][sys_read] bufAddr type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_read] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory (with type casting offset)
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr + (sizeof(std::int32_t)/sizeof(char)-1) >= store.mems.size()){
        throw Exception("[syscall][sys_read] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *bufPtr = memoryData += bufAddr->data.i32;
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_read] fd type is not i32.", coreStack);
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
    // Pop out count
    Value *count = (Value *)coreStack.pop().data;
    if(count->type != i32){
        throw Exception("[syscall][sys_write] count type is not i32.", coreStack);
    }
    // Pop out buf addr
    Value *bufAddr = (Value *)coreStack.pop().data;
    if(bufAddr->type != i32){
        throw Exception("[syscall][sys_write] bufAddr type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_write] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr + (sizeof(std::int32_t)/sizeof(char)-1) >= store.mems.size()){
        throw Exception("[syscall][sys_write] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *bufPtr = memoryData += bufAddr->data.i32;
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_write] fd type is not i32.", coreStack);
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
    // Pop out flags 
    Value *flags = (Value *)coreStack.pop().data;
    if(flags->type != i32){
        throw Exception("[syscall][sys_open] Flags type is not i32.", coreStack);
    }
    // Pop out address of Pathname
    Value *pathnameAddr = (Value *)coreStack.pop().data;
    if(pathnameAddr->type != i32){
        throw Exception("[syscall][sys_open] PathnameAddr type is not i32.", coreStack);
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
        throw Exception("[syscall][sys_close] fd type is not i32.", coreStack);
    }
    // Sys_Close
    if(close((int)fd->data.i32) == -1){
        throw Exception("[syscall][sys_close] System call failed.", coreStack);
    }
    delete fd;
}

void Call::sysPipe(Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 2){
        throw Exception("[syscall][sys_pipe] No enough value in the stack.", coreStack);
    }
    /* Pipe using 2 elements array as start and end
        - [0]: end
        - [1]: start
    */
    // Pop out the start point of pipe (First)
    Value *start = (Value *)coreStack.pop().data;
    if(start->type != i32){
        throw Exception("[syscall][sys_pipe] Start point type is not i32.", coreStack);
    }
    // Pop out the end point of pipe (Second)
    Value *end = (Value *)coreStack.pop().data;
    if(end->type != i32){
        throw Exception("[syscall][sys_pipe] End point type is not i32.", coreStack);
    }
    // Get into the int array
    int pipefd[2];
    pipefd[0] = (std::int32_t)end->data.i32;
    pipefd[1] = (std::int32_t)start->data.i32;
    // Implement pipe
    pipe(pipefd);
    // Clean
    delete start;
    delete end;
}

void Call::sysDup(Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 2){
        throw Exception("[syscall][sys_dup] No enough value in the stack.", coreStack);
    }
    // Pop out the oldfd
    Value *oldfd = (Value *)coreStack.pop().data;
    if(oldfd->type != i32){
        throw Exception("[syscall][sys_pipe] Start point type is not i32.", coreStack);
    }
    // Getting the new file descriptor
    std::int32_t newfd = dup((std::int32_t)oldfd->data.i32); 
    if(errno == EBADF){
        throw Exception("[syscall][sys_pipe] Old file descriptor isn't an open file descriptor", coreStack);
    }
    // Push new fd into coreStack
    coreStack.push(newfd);
    // Clean
    delete oldfd;
}

void Call::sysPoll(Store &store,Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 3){
        throw Exception("[syscall][sys_poll] No enough value in the stack.", coreStack);
    }
    /*  Poll()
        args - (struct pollfd *) fds, (nfds_t) nfds, (int) timeout
    */
    Value *timeout = (Value *)coreStack.pop().data;
    if(timeout->type != i32){
        throw Exception("[syscall][sys_poll] timeout type is not i32.", coreStack);
    }
    Value *nfds = (Value *)coreStack.pop().data;
    if(nfds->type != i32){
        throw Exception("[syscall][sys_poll] nfds type is not i32.", coreStack);
    }
    Value *fdsAddr = (Value *)coreStack.pop().data;
    if(fdsAddr->type != i32){
        throw Exception("[syscall][sys_poll] fds type is not i32.", coreStack);
    }
    // Fetch data from mem
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_poll] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr + (sizeof(struct pollfd)/sizeof(char)-1) >= store.mems.size()){
        throw Exception("[syscall][sys_poll] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *fdsPtr = memoryData += fdsAddr->data.i32;
    // Implementation
    poll((struct pollfd *)fdsPtr,(nfds_t)nfds->data.i32,(int)timeout->data.i32);
    // Clean
    delete timeout;
    delete nfds;
    delete fdsAddr;
}

void Call::sysLseek(Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 1){
        throw Exception("[syscall][sys_lseek] No enough value in the stack.", coreStack);
    }
    // Pop out file descriptor
    Value *fd = (Value *)coreStack.pop().data;
    if(fd->type != i32){
        throw Exception("[syscall][sys_lseek] fd type is not i32.", coreStack);
    }
    // Pop out offset 
    Value *offset = (Value *)coreStack.pop().data;
    if(offset->type != i32){
        throw Exception("[syscall][sys_lseek] offset type is not i32.", coreStack);
    }
    // Pop out Whence
    Value *whence = (Value *)coreStack.pop().data;
    if(whence->type != i32){
        throw Exception("[syscall][sys_lseek] whence type is not i32.", coreStack);
    }
    // lseek
    lseek((std::int32_t)fd->data.i32,(off_t)offset->data.i32,(std::int32_t)whence->data.i32);
    // Clean 
    delete fd;
    delete offset;
    delete whence;
}

void Call::sysAccess(Store &store, Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 2){
        throw Exception("[syscall][sys_access] No enough value in the stack.", coreStack);
    }
    // Pop out mode
    Value *mode = (Value *)coreStack.pop().data;
    if(mode->type != i32){
        throw Exception("[syscall][sys_access] mode type is not i32.", coreStack);
    }
    // Pop out pathname addr
    Value *pathnameAddr = (Value *)coreStack.pop().data;
    if(pathnameAddr->type != i32){
        throw Exception("[syscall][sys_access] pathnameAddr type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_brk] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr >= store.mems.size()){
        throw Exception("[syscall][sys_brk] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *pathnamePtr = memoryData += pathnameAddr->data.i32;
    // Access
    access((const char *)pathnamePtr,(std::int32_t)mode->data.i32);
    // Clean 
    delete mode;
    delete pathnameAddr;
}

void Call::sysBrk(Store &store,Stack &coreStack){
    // Check value count 
    if(coreStack.valueNum < 1){
        throw Exception("[syscall][sys_brk] No enough value in the stack.", coreStack);
    }
    // Get the `program break` addr
    Value *programbreakAddr = (Value *)coreStack.pop().data;
    if(programbreakAddr->type != i32){
        throw Exception("[syscall][sys_brk] fds type is not i32.", coreStack);
    }
    // Step1: Check memory address first 
    if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
        throw Exception("[syscall][sys_brk] No memory exists in this module.", coreStack);
    }
    // Step2: Check memory
    std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
    if(memAddr >= store.mems.size()){
        throw Exception("[syscall][sys_brk] Memory not exists in the store.", coreStack);
    }
    // Step3: Get the pointer
    char *memoryData = store.mems.at(memAddr)->data.data();
    char *programbreakPtr = memoryData += programbreakAddr->data.i32;
    // Brk
    brk((void *)programbreakPtr);
    // Clean 
    delete programbreakAddr;
}

