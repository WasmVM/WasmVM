#include "LinearMemory.h"

linear_memory::linear_memory(){
	// allocate an empty list
	l_memory = new list();
	sp = 0;
	fp = 0;
}

linear_memory::~linear_memory(){
	delete l_memory;
}

int linear_memory::lm_alloc(Value &value){
	l_memory->push_back(value);
	sp++; // increase stack pointer

	return 0;
}

int linear_memory::lm_free(){
	if(sp >= fp){
		// release memory by the scope.

		return 0;
	}
	else{
	
		return 1; // err
	}
}

int linear_memory::lm_free(int length){
	
}


