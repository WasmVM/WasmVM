#ifndef FUNCINST_DEF
#define FUNCINST_DEF

#include <string>
#include <Value.h>
#include <ModuleInst.h>

class FuncInst{
public:
    FuncType type;
    ModuleInst *module;
    Func code;
};

#endif