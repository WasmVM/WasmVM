#ifndef SYSCALL_DEF
#define SYSCALL_DEF

#include <map>
#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Syscode.h>

class Syscall{
public:
	static std::map<std::string, ModuleInst *> *moduleInsts;
	static void initSyscall(std::map<std::string, ModuleInst *> *moduleInsts);
	static void handle(Store &store, Stack &coreStack);
};

#endif