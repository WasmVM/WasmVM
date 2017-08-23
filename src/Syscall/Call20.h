#ifndef CALL20_DEF
#define CALL20_DEF

#include <cstdlib>
#include <cstring>

extern "C"{
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}

#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Exception.h>

namespace Call{
	void sysRead(Store &store, Stack &callStack);
	void sysWrite(Store &store, Stack &callStack);
	void sysOpen(Store &store, Stack &callStack);
	void sysClose(Stack &callStack);
}

#endif