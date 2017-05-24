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

}

int linear_memory::lm_free(int length){

}


