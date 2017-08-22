#ifndef SYSCALL_DEF
#define SYSCALL_DEF

#include <map>
#include <Instance.h>
#include <Exception.h>
#include <Store.h>
#include <Stack.h>
#include <Syscode.h>

// Include Implementation of Syscall
#include <Call20.h>
#include <Call40.h>

class Syscall{
public:
	static std::map<std::string, ModuleInst *> *moduleInsts;
	static void initSyscall(std::map<std::string, ModuleInst *> *moduleInsts);
	static void handle(Store &store, Stack &coreStack);
};

#endif