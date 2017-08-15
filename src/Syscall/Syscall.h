#ifndef SYSCALL_DEF
#define SYSCALL_DEF

#include <map>
#include <Instance.h>
#include <Store.h>
#include <Stack.h>

class Syscall{
	static std::map<std::string, ModuleInst *> *moduleInsts;
	static void setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts);
	static void handle(Stack &coreStack, Store &store);
};

#endif