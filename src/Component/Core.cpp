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