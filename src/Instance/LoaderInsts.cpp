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
