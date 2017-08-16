#include <Syscall.h>

std::map<std::string, ModuleInst *>* Syscall::moduleInsts = nullptr;
void Syscall::setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts){
	Syscall::moduleInsts = moduleInsts;
}