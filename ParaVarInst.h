#ifndef PARA_VAR_INST
#define PARA_VAR_INST

#include "commonTypes.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "memory.h"

using namespace std;

namespace Instruction{
  void para_drop(OperandStack &opStack);
  void para_select(OperandStack &opStack);
  void get_local(OperandStack &opStack, LocalStack &locals, Memory memory);
  void set_local(OperandStack &opStack, LocalStack &locals, Memory memory);
  void tee_local(OperandStack &opStack, LocalStack &locals, Memory memory);
  void get_global();
  void set_global();
}

#endif
