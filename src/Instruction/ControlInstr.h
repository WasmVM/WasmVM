#ifndef CONTROLINSTR_DEF
#define CONTROLINSTR_DEF

#include <cstdint>
#include <Store.h>
#include <Stack.h>
#include <Instance.h>
#include <ControlException.h>

namespace Instruction{
	void invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack);
}

#endif