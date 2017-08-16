#ifndef VARIABLEINSTR_DEF
#define VARIABLEINSTR_DEF

#include <cstdint>
#include <Store.h>
#include <Stack.h>
#include <Instance.h>
#include <Exception.h>

namespace Instruction{
	void get_local(std::uint32_t index, Stack &coreStack);
	void set_local(std::uint32_t index, Stack &coreStack);
	void tee_local(std::uint32_t index, Stack &coreStack);
	void get_global(std::uint32_t index, Store &store, Stack &coreStack);
	void set_global(std::uint32_t index, Store &store, Stack &coreStack);
}

#endif