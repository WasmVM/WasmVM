#ifndef OPERAND_STACK
#define OPERAND_STACK

#include <stack>

#include "commonTypes.h"

using namespace std;

class OperandStack{
public:

  friend class SystemCall;

  OperandStack();
  void pushVal(const Value &val);
  Value popVal();
  size_t size();
  void append();
  void shrink();

#ifndef TEST
private:
#endif
  stack<stack<Value>> _stacks;
};

#endif
