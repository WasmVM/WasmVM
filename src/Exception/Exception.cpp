#include <Exception.h>

std::map<std::string, ModuleInst *>* Exception::moduleInsts = nullptr;
void Exception::setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts){
	Exception::moduleInsts = moduleInsts;
}

Exception::Exception(std::string desc, Stack &coreStack, ModuleInst *moduleInst):
	desc(desc), coreStack(coreStack), moduleInst(moduleInst)	
{
	if(coreStack.curLabel != nullptr){
		std::stringstream sstream;
		sstream << "(func: " << coreStack.curLabel->funcIdx << ", offset: " << coreStack.curLabel->instrOffset << ") " << desc;
		this->desc = sstream.str();
	}
	if(moduleInst == nullptr && coreStack.curFrame != nullptr){
		this->moduleInst = coreStack.curFrame->moduleInst;
	}
	for(std::map<std::string, ModuleInst *>::iterator moduleIt = moduleInsts->begin(); moduleIt != moduleInsts->end(); ++moduleIt){
		if(moduleIt->second == this->moduleInst){
			this->desc = moduleIt->first + " " + this->desc;
			break;
		}
	}
}
std::string Exception::stackTrace(){
	return "";
}