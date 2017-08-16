#ifndef CONTROLINSTR_DEF
#define CONTROLINSTR_DEF

#include <cstdint>
#include <vector>
#include <iostream>
#include <Store.h>
#include <Stack.h>
#include <Instance.h>
#include <Exception.h>
#include <Bincode.h>
#include <Util.h>

namespace Instruction{
	void ctrl_unreachable(Store &store, Stack &coreStack);
	void ctrl_block(std::vector<char> &funcBody, Stack &coreStack, char blocktype);
	void ctrl_loop(Stack &coreStack, char blocktype);
	void ctrl_if(std::vector<char> &funcBody, Stack &coreStack, char blocktype);
	void ctrl_else(Stack &coreStack);
	void ctrl_br(std::uint32_t depth, Stack &coreStack);
	void ctrl_br_if(std::uint32_t depth, Stack &coreStack);
	void ctrl_br_table(std::vector<std::uint32_t> &depths, Stack &coreStack);
	void ctrl_return(Stack &coreStack);
	void ctrl_call(std::uint32_t funcAddr, Store &store, Stack &coreStack);
	void ctrl_call_indirect(std::uint32_t typeidx, Store &store, Stack &coreStack);
	void ctrl_end(Stack &coreStack);
	void invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack, ModuleInst *moduleInst = nullptr);
}

#endif