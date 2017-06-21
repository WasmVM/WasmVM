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

void LocalStack::append(uint64_t pc){
  _localsValues.push(vector<Value>());
  _localIndices.push(LocalIndices(pc));
}

bool LocalStack::shrink(){
  _localsValues.pop();
  _localIndices.pop();
  if(_localIndices.size() == 0){
    return true;
  }else{
    return false;
  }
}

void LocalStack::push_index(LocalIndex index){
  _localIndices.top().indices.push(index);
}

LocalIndex LocalStack::pop_index(){
  _localsValues.top().resize(_localIndices.top().indices.top().index);
  LocalIndex ret = _localIndices.top().indices.top();
  _localIndices.top().indices.pop();
  return ret;
}

uint64_t &LocalStack::get_PC(){
  return _localIndices.top().pc;
}