#ifndef PARA_VAR_INST
#define PARA_VAR_INST

#include "commonTypes.h"
#include "OperandStack.h"

using namespace std;

namespace Instruction{
  void para_drop(OperandStack &opStack);
  void para_select(OperandStack &opStack);
  void get_local();
  void set_local();
  void tee_local();
  void get_global();
  void set_global();
}

#endif
