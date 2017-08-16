#include <Store.h>

Store::~Store(){
	// Function
	for(std::vector<FuncInst *>::iterator funcIt = funcs.begin(); funcIt != funcs.end(); ++funcIt){
		delete *funcIt;
	}
	// Table
	for(std::vector<TableInst *>::iterator tableIt = tables.begin(); tableIt != tables.end(); ++tableIt){
		delete *tableIt;
	}
	// Memory
	for(std::vector<MemInst *>::iterator memIt = mems.begin(); memIt != mems.end(); ++memIt){
		delete *memIt;
	}
	// Global
	for(std::vector<GlobalInst *>::iterator globalIt = globals.begin(); globalIt != globals.end(); ++globalIt){
		delete *globalIt;
	}
}