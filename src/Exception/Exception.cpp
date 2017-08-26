/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

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
