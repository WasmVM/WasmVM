#ifndef CONTROLINSTR_DEF
#define CONTROLINSTR_DEF

#include <cstdint>
#include <Store.h>
#include <Stack.h>
#include <Instance.h>
#include <Exception.h>

namespace Instruction{
	//[Later]TODO: void ctrl_unreachable(Store &store, Stack &coreStack);
	//TODO: void ctrl_block(Stack &coreStack);
	//TODO: void ctrl_loop(Stack &coreStack);
	//[Later]TODO: void ctrl_if(Stack &coreStack);
	//[Later]TODO: void ctrl_else(Stack &coreStack);
	//TODO: void ctrl_br(Stack &coreStack);
	//TODO: void ctrl_br_if(Stack &coreStack);
	//TODO: void ctrl_return(Stack &coreStack);
	//TODO: void ctrl_call(Stack &coreStack);
	//TODO: void ctrl_call_indirect(Stack &coreStack);
	void ctrl_end(Stack &coreStack);
	void invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack, ModuleInst *moduleInst = nullptr);
}

#endif