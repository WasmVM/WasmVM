/*
Copyright Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

#include <LoaderInsts.h>

Module::~Module(){
	delete start;
	for(std::vector<Import>::iterator impIt = imports.begin(); impIt != imports.end(); ++impIt){
		switch(impIt->kind){
			case ExternType::func:
				delete impIt->desc.funcType;
			break;
			case ExternType::table:
				delete impIt->desc.table;
			break;
			case ExternType::mem:
				delete impIt->desc.mem;
			break;
			case ExternType::global:
				delete impIt->desc.global;
			break;
		}
	}
}
