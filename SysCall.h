#ifndef SYSTEM_CALL
#define SYSTEM_CALL

#include <cstdio>
#include <cstdlib>

#include "OperandStack.h"
#include "Memory.h"
#include "LocalStack.h"

using namespace std;

class SystemCall{
public:
  SystemCall(OperandStack &opStack, LocalStack &locals, Memory &memory);
  /*** System calls ***/
  void printOperand(OperandStack &stack);
};

#endif