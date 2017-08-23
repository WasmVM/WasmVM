#ifndef CALL20_DEF
#define CALL20_DEF

#include <Instance.h>
#include <Store.h>
#include <Stack.h>

namespace Call{
	void sysRead(Store &store, Stack &callStack);
	void sysWrite(Store &store, Stack &callStack);
	void sysOpen(Store &store, Stack &callStack);
	void sysClose(Store &store, Stack &callStack);
}

#endif