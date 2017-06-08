#include "OperandStack.h"

OperandStack::OperandStack(){

}

void OperandStack::push(const Value &val){
  _stack.push(val);
}

Value OperandStack::pop(){
  Value ret = _stack.top();
  _stack.pop();
  return ret;
}

size_t OperandStack::size(){
  return _stack.size();
}
