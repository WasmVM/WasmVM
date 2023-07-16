// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Drop&){
    stack.frames.top().labels.top().values.pop();
}
void RunVisitor::operator()(Instr::Select& instr){
    Label& label = stack.frames.top().labels.top();
    // Get operand
    bool operand = (std::get<i32_t>(label.values.top()) == 0);
    label.values.pop();
    // Pop values
    std::vector<Value> val1;
    std::vector<Value> val2;
    if(instr.valtypes.empty()){
        val2.emplace_back(label.values.top());
        label.values.pop();
        val1.emplace_back(label.values.top());
        label.values.pop();
    }else{
        for(ValueType type : instr.valtypes){
            if(label.values.top().index() != (size_t)type){
                throw Exception::invalid_value();
            }
            val2.emplace_back(label.values.top());
            label.values.pop();
        }
        for(ValueType type : instr.valtypes){
            if(label.values.top().index() != (size_t)type){
                throw Exception::invalid_value();
            }
            val1.emplace_back(label.values.top());
            label.values.pop();
        }
    }
    // Push values
    label.values.insert(operand ? val2 : val1);
}