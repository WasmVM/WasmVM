#ifndef TRAP_HANDLER
#define TRAP_HANDLER

#include <cstdio>
#include <cstdlib>

#include "OperandStack.h"
#include "Memory.h"

using namespace std;

namespace TrapHandler{

void handle(OperandStack &stack, Memory &memory);

/*** System calls ***/

void printf(OperandStack &stack, Memory &memory);
  
};

#endif