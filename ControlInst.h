#ifndef CONTROL_INST
#define CONTROL_INST

#include <cstdint>
#include <stack>

#include "commonTypes.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "Memory.h"
#include "SysCall.h"

using namespace std;

namespace Instruction{
  class BlockExtra{
  public:
    uint32_t retType;
  };
  class LoopExtra{
  public:
    uint64_t pc;
    uint32_t retType;
  };
  class IfExtra{
  public:
    uint64_t endPos, elsePos;
    uint32_t retType;
  };
  void ctrl_nop();
  void ctrl_unreachable(OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_block(LocalStack &locals, uint32_t retType);
  void ctrl_loop(LocalStack &locals, uint32_t retType);
  void ctrl_if(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t retType);
  void ctrl_else(LocalStack &locals);
  void ctrl_br(LocalStack &locals, Memory &memory, uint32_t label);
  void ctrl_br_if(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t label);
  void ctrl_return();
  void ctrl_call(uint32_t entry, OperandStack &opStack, LocalStack &locals, Memory &memory);
  void ctrl_end(OperandStack &opStack, LocalStack &locals, bool &halted);

  uint32_t bypass(Memory &memory, uint64_t &pc);
}

#endif