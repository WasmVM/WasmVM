#ifndef CONTROL_INST
#define CONTROL_INST

#include <stack>

#include "commonTypes.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "Memory.h"
#include "SysCall.h"

using namespace std;

namespace Instruction{
  class LoopExtra{
  public:
    uint64_t pc;
    uint32_t retType;
  };
  void ctrl_nop ();
  void ctrl_unreachable(OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_block(LocalStack &locals);
  void ctrl_loop(LocalStack &locals, uint32_t retType);
  void ctrl_if();
  void ctrl_br();
  void ctrl_br_if();
  void ctrl_return();
  void ctrl_call(uint32_t entry, OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_end(OperandStack &opStack, LocalStack &locals, bool &halted);
}

#endif