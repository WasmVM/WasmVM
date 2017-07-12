#include "LocalStack.h"

LocalStack::LocalStack(){

}

Value LocalStack::get_local(uint32_t index){
  return _localValues.top().at(index);
}

void LocalStack::set_local(uint32_t index, Value val){
  if(index < _localValues.top().size()){
    _localValues.top().at(index) = val;
  }else{
    // TODO: Exception Handling
  }
}

void LocalStack::push_local(Value val){
  _localValues.top().push_back(val);
}

Value LocalStack::tee_local(uint32_t index, Value val){
  if(index < _localValues.top().size()){
    _localValues.top().at(index) = val;
  }else if(index == _localValues.top().size()){
    _localValues.top().push_back(val);
  }else{
    // TODO: Exception Handling
  }
  return val;
}

void LocalStack::append(uint64_t pc){
  _localValues.push(vector<Value>());
  _localIndices.push(LocalIndices(pc));
}

bool LocalStack::shrink(){
  _localValues.pop();
  _localIndices.pop();
  if(_localIndices.size() == 0){
    return true;
  }else{
    return false;
  }
}

void LocalStack::push_index(IndexType type, void *extra){
  _localIndices.top().indices.push(LocalIndex(type, _localValues.top().size(), extra));
}

LocalIndex LocalStack::pop_index(){
  _localValues.top().resize(_localIndices.top().indices.top().index);
  LocalIndex ret = _localIndices.top().indices.top();
  _localIndices.top().indices.pop();
  return ret;
}

LocalIndex LocalStack::peek_index(){
  return _localIndices.top().indices.top();
}

uint64_t &LocalStack::get_PC(){
  return _localIndices.top().pc;
}