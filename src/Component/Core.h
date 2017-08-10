#ifndef CORE_DEF
#define CORE_DEF

#include <Instance.h>
#include <Store.h>
#include <Stack.h>

class Core{
public:
	Core(Store &store);
	void run(ModuleInst *module);

private:
	Store &store;
	Stack coreStack;
	ModuleInst *module;
};

#endif