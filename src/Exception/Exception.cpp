#include <Exception.h>

std::map<std::string, ModuleInst *>* Exception::moduleInsts = nullptr;
void Exception::setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts){
	Exception::moduleInsts = moduleInsts;
}

Exception::Exception(std::string desc, Stack &coreStack, ModuleInst *moduleInst):
	desc(desc), coreStack(coreStack), moduleInst(moduleInst)	
{
	bool labeled = false;
	for(std::list<StackElem>::iterator stackIt = coreStack.begin(); stackIt != coreStack.end(); ++stackIt){
		if(!labeled && stackIt->type == StackElemType::label){
			labeled = true;
			if(moduleInst == nullptr){
				std::stringstream sstream;
				sstream << "(func: " << ((Label *)stackIt->data)->funcIdx << " , instr: " << ((Label *)stackIt->data)->instrOffset << " ) " << desc;
				this->desc = sstream.str();
			}
		}
		if(moduleInst == nullptr && stackIt->type == StackElemType::frame){
			this->moduleInst = ((Frame *)stackIt->data)->moduleInst;
			break;
		}
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