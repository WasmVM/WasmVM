// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <instances/Stack.hpp>
#include "exception.hpp"
#include "RunVisitor.hpp"

using namespace WasmVM;

void Stack::invoke(FuncInst funcinst, std::vector<Value> args){
    if(args.size() != funcinst.type.params.size()){
        throw Exception::invalid_argument();
    }
    for(size_t i = 0; i < args.size(); ++i){
        if(args[i].index() != funcinst.type.params[i]){
            throw Exception::invalid_argument();
        }
    }
    if(std::holds_alternative<FuncInst::Body>(funcinst.body)){
        // Frame
        FuncInst::Body& body = std::get<FuncInst::Body>(funcinst.body);
        Frame& frame = frames.emplace(body.module);
        frame.end = body.func.body.end();
        // Locals
        frame.locals.insert(frame.locals.end(), args.begin(), args.end());
        for(ValueType type : body.func.locals){
            switch(type){
                case ValueType::i32 :
                    frame.locals.emplace_back((i32_t)0);
                break;
                case ValueType::i64 :
                    frame.locals.emplace_back((i64_t)0);
                break;
                case ValueType::f32 :
                    frame.locals.emplace_back((f32_t)0.0f);
                break;
                case ValueType::f64 :
                    frame.locals.emplace_back((f64_t)0.0);
                break;
                case ValueType::funcref :
                    frame.locals.emplace_back(funcref_t(std::nullopt));
                break;
                case ValueType::externref :
                    frame.locals.emplace_back((externref_t)nullptr);
                break;
            }
        }
        // Label
        Label& label = frame.labels.emplace(Label());
        label.arity = funcinst.type.results.size();
        label.continuation = body.func.body.end();
        label.current = body.func.body.begin();
    }else{
        hostfunc_t func = std::get<hostfunc_t>(funcinst.body);
        if(frames.empty() || frames.top().labels.empty()){
            results = func(*this);
        }else{
            frames.top().labels.top().values.insert(func(*this));
        }
    }
}

std::vector<Value> Stack::run(){
    RunVisitor visitor(*this);
    while(!frames.empty() && !frames.top().labels.empty()){
        Frame& frame = frames.top();
        Label& label = frame.labels.top();
        if(label.current == frame.end){
            throw Exception::no_end_of_func();
        }
        std::visit(visitor, *label.current);
    }
    return results;
}

std::vector<Value> Label::Values::get(){
    return std::vector<Value>(c.begin(), c.end());
}

void Label::Values::insert(std::vector<Value> values){
    c.insert(c.end(), values.begin(), values.end());
}

using namespace Exception;
invalid_argument::invalid_argument() : 
    Exception("invalid argument") {}
invalid_results::invalid_results() : 
    Exception("invalid results") {}
unreachable::unreachable() :
    Exception("unreachable") {}
no_end_of_func::no_end_of_func() :
    Exception("function reach end without 'end' instruction") {}