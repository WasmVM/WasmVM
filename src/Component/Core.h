#ifndef CORE_DEF
#define CORE_DEF

#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Exception.h>
#include <ControlInstr.h>
#include <Decoder.h>

#ifdef __linux__
#include <dlfcn.h>
#endif

class Core{
public:
	Core(Store &store);
	void run(ModuleInst *module);

private:
	Store &store;
	Stack coreStack;
};

#endif