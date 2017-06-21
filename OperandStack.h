#ifndef OPERAND_STACK
#define OPERAND_STACK

#include <stack>

#include "commonTypes.h"

using namespace std;

class OperandStack{
public:
  OperandStack();
  void push(const Value &val);
  Value pop();
  size_t size();
  void append();
  void shrink();

#ifndef TEST
private:
#endif
  stack<stack<Value>> _stacks;
};

#endif
