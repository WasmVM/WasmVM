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
      _localsValues.top().push(val);
    }else{
      // TODO: Exception Handling
    }
  }

  Value LocalStack::tee_local(uint32_t index, Value val){
    if(index < _localsValues.top().size()){
      _localsValues.top().at(index) = val;
    }else if(index == _localsValues.top().size()){
      _localsValues.top().push(val);
    }else{
      // TODO: Exception Handling
    }
    return val;
  }

  void LocalStack::appendValues(){
    _localsValues.push(vector<Value>());
  }

  void LocalStack::popValues(){
    _localsValues.pop();
  }

  void LocalStack::appendIndices(){
    _localIndices.push(_localIndices.size() - 1);
  }

  void LocalStack::popIndices(){
    _localIndices.pop();
  }