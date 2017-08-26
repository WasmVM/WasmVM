/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

#include <Core.h>

Core::Core(Store &store): store(store){
}

void Core::run(ModuleInst *moduleInst){
	// Invoke start function
	if(moduleInst->start == nullptr){
		throw Exception("No start function in this module.", coreStack, moduleInst);
	}
	Instruction::invoke(*(moduleInst->start), store, coreStack, moduleInst);
	// Run
	while(coreStack.curLabel != nullptr){
		Decoder::decode(store, coreStack);
	}
}
