#ifndef CALL40_DEF
#define CALL40_DEF

#include <cstdlib>
#include <cstring>
extern "C"{
#include <signal.h>
#include <unistd.h>
#include <errno.h>
}

#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Exception.h>

namespace Call{
	void sysExit(Stack &coreStack);
	void sysKill(Stack &coreStack);
	void sysPause(Stack &coreStack);
	void sysGetpid(Stack &coreStack);
	void sysExecve(Store &store, Stack &coreStack);
}

#endif