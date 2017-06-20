#ifndef SYSTEM_CALL
#define SYSTEM_CALL

#include <cstdio>
#include <cstdlib>

#include "OperandStack.h"
#include "Memory.h"

using namespace std;

namespace SystemCall{

void handle(OperandStack &stack, Memory &memory);

/*** System calls ***/

void printf(OperandStack &stack, Memory &memory);
  
};

#endif