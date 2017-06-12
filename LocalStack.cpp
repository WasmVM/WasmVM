#include "LocalStack.h"

LocalStack::LocalStack(){

}

Value LocalStack::get_local(uint32_t index){
  return _localsValues.top().at(index);
}

void LocalStack::set_local(uint32_t index, Value val){
  if(index < _localsValues.top().size()){
    _localsValues.top().at(index) = val;
  }else if(index == _localsValues.top().size()){
    _localsValues.top().push_back(val);
  }else{
    // TODO: Exception Handling
  }
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

void LocalStack::pop_Values(){
  _localsValues.pop();
}

void LocalStack::append_Indices(){
  _localIndices.push(_localIndices.size() - 1);
}

void LocalStack::pop_Indices(){
  _localIndices.pop();
}