// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "state.hpp"
#include "exception.hpp"

using namespace WasmVM;
using namespace Validate;

const std::vector<ValueType>& Validate::State::CtrlFrame::types(){
    if(opcode == Opcode::Loop){
        return type.params;
    }else{
        return type.results;
    }
}

void Validate::State::push(StackValue val){
    vals.emplace_back(val);
}

void Validate::State::push(std::vector<StackValue> vlist){
    for(StackValue& val : vlist){
        vals.emplace_back(val);
    }
}
void Validate::State::push(std::vector<ValueType> vlist){
    for(ValueType& val : vlist){
        vals.emplace_back(val);
    }
}

void Validate::State::push(Opcode::opcode_t opcode, FuncType type){
    ctrls.emplace_back(opcode, type, vals.size());
    std::vector<StackValue> values;
    std::transform(type.params.begin(), type.params.end(), std::back_inserter(values), [](ValueType t){
        return StackValue(t);
    });
    push(values);
}

template<> State::StackValue State::pop<State::StackValue>(){
    CtrlFrame& ctrl = ctrls.back();
    if(vals.size() == ctrl.height){
        if(ctrl.unreachable){
            return std::monostate();
        }else{
            throw Exception::Validate_empty_stack("empty validate value stack");
        }
    }
    State::StackValue result = vals.back();
    vals.pop_back();
    return result;
}
State::StackValue State::pop(State::StackValue expect){
    State::StackValue actual = vals.back();
    vals.pop_back();
    if(std::holds_alternative<std::monostate>(actual)){
        return expect;
    }
    if(std::holds_alternative<std::monostate>(expect)){
        return actual;
    }
    if(std::get<ValueType>(actual) != std::get<ValueType>(expect)){
        throw Exception::Validate_type_not_match("validate value type not match");
    }
    return actual;
}
std::vector<State::StackValue> State::pop(std::vector<ValueType> expects){
    std::vector<StackValue> popped;
    for(auto it = expects.rbegin(); it != expects.rend(); it = std::next(it)){
        popped.emplace_back(pop(*it));
    }
    return popped;
}
template<> State::CtrlFrame State::pop<State::CtrlFrame>(){
    if(ctrls.empty()){
        throw Exception::Validate_empty_stack("empty validate control stack");
    }
    CtrlFrame& ctrl = ctrls.back();
    pop(ctrl.type.results);
    if(vals.size() != ctrl.height){
        throw Exception::Validate_type_not_match("validate frame type not match");
    }
    ctrls.pop_back();
    return ctrl;
}
void State::unreachable(){
    CtrlFrame& ctrl = ctrls.back();
    vals.resize(ctrl.height);
    ctrl.unreachable = true;
}