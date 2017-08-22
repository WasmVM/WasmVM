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