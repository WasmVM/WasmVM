#ifndef CONTROLEXCEPTION_DEF
#define CONTROLEXCEPTION_DEF

#include <string>
#include <Exception.h>

class ControlException : public Exception{
public:
    ControlException(std::string instr, std::string desc, Stack &coreStack, ModuleInst *moduleInst = nullptr);
};

#endif
