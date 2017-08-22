#ifndef CALL40_DEF
#define CALL40_DEF

#include <cstdlib>
extern "C"{
#include <signal.h>
#include <unistd.h>
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
}

#endif