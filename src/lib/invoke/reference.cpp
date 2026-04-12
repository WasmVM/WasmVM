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
    }else if(std::holds_alternative<i31ref_t>(val)){
        label.values.emplace<i32_t>(std::get<i31ref_t>(val).has_value() ? 0 : 1);
    }else{
        throw Exception::invalid_reference();
    }
}
void RunVisitor::operator()(Instr::Ref_func& instr){
    Frame& frame = stack.frames.top();
    Label& label = frame.labels.top();
    label.values.emplace<funcref_t>(frame.module.funcaddrs[instr.index]);
}

static bool ref_is_null(const Value& val){
    if(std::holds_alternative<funcref_t>(val)){
        return !std::get<funcref_t>(val).has_value();
    }
    if(std::holds_alternative<externref_t>(val)){
        return std::get<externref_t>(val) == nullptr;
    }
    if(std::holds_alternative<i31ref_t>(val)){
        return !std::get<i31ref_t>(val).has_value();
    }
    return false;
}


// heaptype encoding as signed LEB128: -16=func, -17=extern, -20=i31
static bool ref_matches_heaptype(const Value& val, int32_t heaptype){
    if(std::holds_alternative<funcref_t>(val)){
        return heaptype == -16; // func
    }
    if(std::holds_alternative<externref_t>(val)){
        return heaptype == -17; // extern
    }
    if(std::holds_alternative<i31ref_t>(val)){
        return heaptype == -20; // i31
    }
    return false;
}

void RunVisitor::operator()(Instr::Ref_as_non_null&){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    if(ref_is_null(val)){
        throw Exception::null_reference();
    }
    // value already on stack, nothing to change
}

void RunVisitor::operator()(Instr::Ref_eq&){
    Label& label = stack.frames.top().labels.top();
    Value val2 = label.values.top(); label.values.pop();
    Value val1 = label.values.top(); label.values.pop();
    bool equal = false;
    if(std::holds_alternative<funcref_t>(val1) && std::holds_alternative<funcref_t>(val2)){
        equal = std::get<funcref_t>(val1) == std::get<funcref_t>(val2);
    } else if(std::holds_alternative<externref_t>(val1) && std::holds_alternative<externref_t>(val2)){
        equal = std::get<externref_t>(val1) == std::get<externref_t>(val2);
    }
    label.values.emplace<i32_t>(equal ? 1 : 0);
}

void RunVisitor::operator()(Instr::Ref_test& instr){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top(); label.values.pop();
    bool result = !ref_is_null(val) && ref_matches_heaptype(val, instr.heaptype);
    label.values.emplace<i32_t>(result ? 1 : 0);
}

void RunVisitor::operator()(Instr::Ref_test_null& instr){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top(); label.values.pop();
    bool result = ref_is_null(val) || ref_matches_heaptype(val, instr.heaptype);
    label.values.emplace<i32_t>(result ? 1 : 0);
}

void RunVisitor::operator()(Instr::Ref_cast& instr){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    if(ref_is_null(val) || !ref_matches_heaptype(val, instr.heaptype)){
        throw Exception::cast_failure();
    }
    // value already on stack
}

void RunVisitor::operator()(Instr::Ref_cast_null& instr){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    if(!ref_is_null(val) && !ref_matches_heaptype(val, instr.heaptype)){
        throw Exception::cast_failure();
    }
    // value already on stack
}

void RunVisitor::operator()(Instr::Ref_i31&){
    Label& label = stack.frames.top().labels.top();
    i32_t i = std::get<i32_t>(label.values.top());
    label.values.pop();
    uint32_t bits = (uint32_t)i & 0x7FFFFFFFU;
    label.values.emplace(Value(i31ref_t(bits)));
}

void RunVisitor::operator()(Instr::I31_get_s&){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    label.values.pop();
    if(!std::holds_alternative<i31ref_t>(val) || !std::get<i31ref_t>(val).has_value()){
        throw Exception::null_reference();
    }
    uint32_t bits = *std::get<i31ref_t>(val);
    // sign-extend bit 30 to bit 31
    i32_t result = (bits & 0x40000000U) ? (i32_t)(bits | 0x80000000U) : (i32_t)bits;
    label.values.emplace(Value(result));
}

void RunVisitor::operator()(Instr::I31_get_u&){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    label.values.pop();
    if(!std::holds_alternative<i31ref_t>(val) || !std::get<i31ref_t>(val).has_value()){
        throw Exception::null_reference();
    }
    label.values.emplace(Value((i32_t)*std::get<i31ref_t>(val)));
}

void RunVisitor::operator()(Instr::Any_convert_extern&){
    // any.convert_extern: externref -> anyref (treated as externref in this impl)
    // value stays on stack unchanged
}

void RunVisitor::operator()(Instr::Extern_convert_any&){
    // extern.convert_any: anyref -> externref (treated as externref in this impl)
    // value stays on stack unchanged
}

void RunVisitor::operator()(Instr::Return_call_ref& instr){
    Frame& frame = stack.frames.top();
    Label& label = frame.labels.top();
    // Pop funcref (on top of stack, above args)
    Value ref_val = label.values.top();
    label.values.pop();
    if(!std::holds_alternative<funcref_t>(ref_val)){
        throw Exception::invalid_reference();
    }
    funcref_t funcref = std::get<funcref_t>(ref_val);
    if(!funcref.has_value()){
        throw Exception::null_reference();
    }
    index_t funcaddr = funcref.value();
    FuncType& type = stack.store.funcs[funcaddr].type;
    if(type != frame.module.types[instr.index]){
        throw Exception::function_type_unmatch();
    }
    std::vector<Value> args;
    if(type.params.size() > 0){
        args.resize(type.params.size());
        for(int i = type.params.size() - 1; i >= 0; --i){
            args[i] = label.values.top();
            label.values.pop();
        }
    }
    // Pop current frame (tail call)
    stack.frames.pop();
    stack.invoke(funcaddr, args);
}

static void do_branch(Stack& stack, index_t labelidx){
    Frame& frame = stack.frames.top();
    std::vector<Value> values = frame.labels.top().values.get();
    for(index_t i = labelidx; i > 0; --i){
        frame.labels.pop();
    }
    Label& label = frame.labels.top();
    if(label.arity > 0){
        label.values.insert(std::vector<Value>(values.end() - label.arity, values.end()));
    }
    label.pc->current = label.pc->continuation;
}

void RunVisitor::operator()(Instr::Br_on_null& instr){
    Frame& frame = stack.frames.top();
    Value val = frame.labels.top().values.top();
    frame.labels.top().values.pop();
    if(ref_is_null(val)){
        // null: branch (ref consumed)
        do_branch(stack, instr.index);
    }else{
        // non-null: push ref back, continue
        frame.labels.top().values.emplace(val);
    }
}

void RunVisitor::operator()(Instr::Br_on_non_null& instr){
    Frame& frame = stack.frames.top();
    Value val = frame.labels.top().values.top();
    if(!ref_is_null(val)){
        // non-null: branch with ref as part of branch values
        do_branch(stack, instr.index);
    }else{
        // null: drop ref, continue
        frame.labels.top().values.pop();
    }
}

void RunVisitor::operator()(Instr::Br_on_cast& instr){
    Frame& frame = stack.frames.top();
    Value val = frame.labels.top().values.top();
    bool is_null = ref_is_null(val);
    bool matches = (instr.dst_nullable && is_null) || (!is_null && ref_matches_heaptype(val, instr.dst_heaptype));
    if(matches){
        // Branch: ref stays on stack as part of branch values
        do_branch(stack, instr.labelidx);
    }
    // else: no branch, ref stays on stack
}

void RunVisitor::operator()(Instr::Br_on_cast_fail& instr){
    Frame& frame = stack.frames.top();
    Value val = frame.labels.top().values.top();
    bool is_null = ref_is_null(val);
    bool matches = (instr.dst_nullable && is_null) || (!is_null && ref_matches_heaptype(val, instr.dst_heaptype));
    if(!matches){
        // Branch: ref stays on stack as part of branch values
        do_branch(stack, instr.labelidx);
    }
    // else: no branch, ref stays on stack
}