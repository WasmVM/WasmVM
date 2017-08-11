#include <ControlException.h>

ControlException::ControlException(std::string instr, std::string desc, Stack &coreStack, ModuleInst *moduleInst):
    Exception(desc, coreStack, moduleInst)
{
    this->desc += " (" + instr + ")";
}