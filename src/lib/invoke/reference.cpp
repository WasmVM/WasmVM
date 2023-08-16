// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Ref_null& instr){
    Label& label = stack.frames.top().labels.top();
    switch(instr.heaptype){
        case RefType::funcref :
            label.values.emplace<funcref_t>(std::nullopt);
        break;
        case RefType::externref :
            label.values.emplace<externref_t>(nullptr);
        break;
    }
}
void RunVisitor::operator()(Instr::Ref_is_null&){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    label.values.pop();
    if(std::holds_alternative<funcref_t>(val)){
        label.values.emplace<i32_t>(!std::get<funcref_t>(val).has_value());
    }else if(std::holds_alternative<externref_t>(val)){
        label.values.emplace<i32_t>(std::get<externref_t>(val) == nullptr);
    }else{
        throw Exception::invalid_reference();
    }
}
void RunVisitor::operator()(Instr::Ref_func& instr){
    Frame& frame = stack.frames.top();
    Label& label = frame.labels.top();
    label.values.emplace<funcref_t>(frame.module.funcaddrs[instr.index]);
}