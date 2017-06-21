#ifndef CONTROL_INST
#define CONTROL_INST

#include <stack>

#include "OperandStack.h"
#include "LocalStack.h"
#include "Memory.h"
#include "SysCall.h"

using namespace std;

namespace Instruction{
  void ctrl_nop ();
  void ctrl_unreachable(OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_block();
  void ctrl_loop();
  void ctrl_if();
  void ctrl_br();
  void ctrl_br_if();
  void ctrl_return();
  void ctrl_call(uint32_t entry, OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_end(OperandStack &opStack, LocalStack &locals, bool &halted);
}

#endif