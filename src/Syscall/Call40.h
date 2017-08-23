#ifndef CALL40_DEF
#define CALL40_DEF

#include <cstdlib>
#include <cstring>
extern "C"{
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
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
	void sysFork(Stack &coreStack);
	void sysVfork(Stack &coreStack);
	void sysSocket(Stack &coreStack);
	void sysShutdown(Stack &coreStack);
	void sysConnect(Store &store, Stack &coreStack);
	void sysBind(Store &store, Stack &coreStack);
}

#endif