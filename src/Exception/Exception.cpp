#include <Exception.h>

std::map<std::string, ModuleInst *>* Exception::moduleInsts = nullptr;
void Exception::setModuleInsts(std::map<std::string, ModuleInst *> *moduleInsts){
	Exception::moduleInsts = moduleInsts;
}

Exception::Exception(std::string desc, Stack &coreStack, ModuleInst *moduleInst):
	desc(desc), coreStack(coreStack), moduleInst(moduleInst), moduleName(""), funcIdx(0), instrOffset(0)	
{
	bool labeled = false;
	for(std::list<StackElem>::iterator stackIt = coreStack.begin(); stackIt != coreStack.end(); ++stackIt){
		if(!labeled && stackIt->type == StackElemType::label){
			funcIdx = ((Label *)stackIt->data)->funcIdx;
			instrOffset = ((Label *)stackIt->data)->instrOffset;
			labeled = true;
		}
		if(moduleInst == nullptr && stackIt->type == StackElemType::frame){
			moduleInst = ((Frame *)stackIt->data)->moduleInst;
			break;
		}
	}
	for(std::map<std::string, ModuleInst *>::iterator moduleIt = moduleInsts->begin(); moduleIt != moduleInsts->end(); ++moduleIt){
		if(moduleIt->second == moduleInst){
			moduleName = moduleIt->first;
		}
	}
}
std::string Exception::stackTrace(){
	
}