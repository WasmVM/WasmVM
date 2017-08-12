#ifndef EXCEPTION_DEF
#define EXCEPTION_DEF

#include <map>
#include <vector>
#include <sstream>
#include <cstdint>
#include <string>
#include <ModuleInst.h>
#include <Stack.h>

class Exception {
public:
    static std::map<std::string, ModuleInst *> *moduleInsts;
    static void setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts);

    Exception(std::string desc, Stack &coreStack, ModuleInst *moduleInst = nullptr);
    std::string stackTrace();

    std::string desc;
    Stack &coreStack;
    ModuleInst *moduleInst;
};

#endif