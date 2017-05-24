#ifndef LINEARMEM
#define LINEARMEM

#include <list>
#include "commonTypes.h" // includes type of operand usage

class linear_memory {
public:
	linear_memory();
	~linear_memory();
	int lm_alloc(Value &value);
	int lm_free(); // default memory free (using sp & fp to free)
	int lm_free(int length);

private:
	// FIXME implement l_memory manager
	std::list<Value> *l_memory; // individual memory
	int sp; // as stack pointer
	int fp; // as frame pointer
}


#endif
