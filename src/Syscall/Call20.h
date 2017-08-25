#ifndef CALL20_DEF
#define CALL20_DEF

#include <cstdlib>
#include <cstring>

extern "C"{
	#include <sys/types.h>
	#include <sys/time.h>
	#include <sys/stat.h>
	#include <sys/select.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <poll.h>
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
	void sysPoll(Store &store, Stack &coreStack);
	void sysLseek(Stack &coreStack);
	void sysAccess(Store &store, Stack &coreStack);
	void sysBrk(Store &store,Stack &coreStack);
	void sysPipe(Stack &coreStack);
	void sysSelect(Store &store, Stack &coreStack);
	void sysDup(Stack &coreStack);
}

#endif