#include "OperandStack.h"

OperandStack::OperandStack(){

}

void OperandStack::push(const Value &val){
  _stacks.top().push(val);
}

Value OperandStack::pop(){
  Value ret = _stacks.top().top();
  _stacks.top().pop();
  return ret;
}

size_t OperandStack::size(){
  return _stacks.size();
}

void OperandStack::append(){
  _stacks.push(stack<Value>());
}

void OperandStack::shrink(){
  _stacks.pop();
}
