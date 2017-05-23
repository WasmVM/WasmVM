#ifndef OPERAND_STACK
#define OPERAND_STACK

#include <stack>

#include "commonTypes.h"

using namespace std;

class OperandStack{
public:
  OperandStack();
  void push();
  pop();
  size();
private:
  stack<Value> _stack;
};

#endif
