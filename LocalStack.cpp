#include "LocalStack.h"

LocalStack::LocalStack(){

}

Value LocalStack::get_local(uint32_t index){
  return _localsValues.top().at(index);
}

void LocalStack::set_local(uint32_t index, Value val){
  if(index < _localsValues.top().size()){
    _localsValues.top().at(index) = val;
  }else{
    // TODO: Exception Handling
  }
}

void LocalStack::push_local(Value val){
  _localsValues.top().push_back(val);
}

Value LocalStack::tee_local(uint32_t index, Value val){
  if(index < _localsValues.top().size()){
    _localsValues.top().at(index) = val;
  }else if(index == _localsValues.top().size()){
    _localsValues.top().push_back(val);
  }else{
    // TODO: Exception Handling
  }
  return val;
}

void LocalStack::append_Values(){
  _localsValues.push(vector<Value>());
}

void LocalStack::shrink_Values(){
  _localsValues.pop();
}

void LocalStack::append_Indices(){
  _localIndices.push(stack<LocalIndex>());
}

void LocalStack::shrink_Indices(){
  _localIndices.pop();
}

void LocalStack::push_index(LocalIndex index){
  _localIndices.top().push(index);
}

LocalIndex LocalStack::pop_index(){
  _localsValues.top().resize(_localIndices.top().top().index);
  LocalIndex ret = _localIndices.top().top();
  _localIndices.top().pop();
  return ret;
}

uint64_t &LocalStack::get_PC(){
  return _localIndices.top().top().pc;
}