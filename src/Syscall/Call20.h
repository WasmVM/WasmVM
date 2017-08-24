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
	void sysRead(Store &store, Stack &coreStack);
	void sysWrite(Store &store, Stack &coreStack);
	void sysOpen(Store &store, Stack &coreStack);
	void sysClose(Stack &coreStack);
}

#endif