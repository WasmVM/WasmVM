/*
Copyright 2017 Luis Hsu, Kevin Cyu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/
#include <Call40.h>

void Call::sysExit(Stack &coreStack){
	// Check value count
	if(coreStack.valueNum < 1){
		throw Exception("[sys_exit] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[sys_exit] Operand type is not i32.", coreStack);
	}
	// Sys_exit
	exit(operand->data.i32);
	// Clean
	delete operand;
}
void Call::sysKill(Stack &coreStack){
	// Check value count
	if(coreStack.valueNum < 2){
		throw Exception("[sys_kill] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *sig = (Value *)coreStack.pop().data;
	if(sig->type != i32){
		throw Exception("[sys_kill] signal type is not i32.", coreStack);
	}
	Value *pid = (Value *)coreStack.pop().data;
	if(pid->type != i32){
		throw Exception("[sys_kill] pid type is not i32.", coreStack);
	}
	// Sys_kill
	std::int32_t ret = kill(pid->data.i32, sig->data.i32);
	coreStack.push(ret);
	// Clean
	delete sig;
	delete pid;
}
void Call::sysPause(Stack &coreStack){
	// Sys_pause
	std::int32_t ret = pause();
	coreStack.push(ret);
}
void Call::sysGetpid(Stack &coreStack){
	// Sys_getpid
	std::int32_t ret = getpid();
	coreStack.push(ret);
}
void Call::sysExecve(Store &store, Stack &coreStack){
	// Check value count
	if(coreStack.valueNum < 3){
		throw Exception("[sys_execve] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *envAddr = (Value *)coreStack.pop().data;
	Value *argvAddr = (Value *)coreStack.pop().data;
	Value *fileNameAddr = (Value *)coreStack.pop().data;
	if(envAddr->type != i32 || argvAddr->type != i32 || fileNameAddr->type != i32){
		throw Exception("[sys_execve] value types are not i32.", coreStack);
	}
	// Get memory address
	char *memoryData = store.mems.at(0)->data.data();
	char *fileNamePtr = memoryData += fileNameAddr->data.i32;
	char *argvPtr = memoryData += argvAddr->data.i32;
	char *envPtr = memoryData += envAddr->data.i32;
	// Get argv
	std::vector<char *> argv;
	do{
		argv.push_back(argvPtr);
		argvPtr += strlen(argvPtr) + 1;
	}while(*(std::int32_t *)argvPtr);
	argv.push_back(NULL);
	// Get env
	std::vector<char *> env;
	do{
		env.push_back(envPtr);
		envPtr += strlen(envPtr) + 1;
	}while(*(std::int32_t *)envPtr);
	env.push_back(NULL);
	// Sys_execve
	std::int32_t ret = execve(fileNamePtr, (char**)argv.data(), (char**)env.data());
	if(errno){
		perror("Error:");
	}
	coreStack.push(ret);
	// Clean
	delete envAddr;
	delete argvAddr;
	delete fileNameAddr;
}