#ifndef CONTROL_INST
#define CONTROL_INST

#include <stack>

#include "OperandStack.h"
#include "LocalStack.h"
#include "Memory.h"

using namespace std;

namespace Instruction{
  void ctrl_nop ();
  void ctrl_unreachable();
  void ctrl_block();
  void ctrl_loop();
  void ctrl_if();
  void ctrl_br();
  void ctrl_br_if();
  void ctrl_return();
  void ctrl_call(uint32_t entry,
    OperandStack &stack,
    LocalStack &locals,
    Memory &memory,
    std::stack<uint64_t> &pcStack);
  void ctrl_end();
}

#endif