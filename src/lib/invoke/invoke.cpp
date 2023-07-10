// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <instances/Stack.hpp>
#include "exception.hpp"
#include "RunVisitor.hpp"

using namespace WasmVM;

void Stack::invoke(index_t funcaddr, std::vector<Value> args){
    // Check funcaddr
    if(funcaddr >= store.funcs.size()){
        throw Exception::func_not_exist();
    }
    // Check func type
    FuncInst& funcinst = store.funcs[funcaddr];
    if(args.size() != funcinst.type.params.size()){
        throw Exception::invalid_argument();
    }
    for(size_t i = 0; i < args.size(); ++i){
        if(args[i].index() != funcinst.type.params[i]){
            throw Exception::invalid_argument();
        }
    }
    // Frame
    Frame& frame = frames.emplace(funcinst.module, funcaddr);
    frame.locals.insert(frame.locals.end(), args.begin(), args.end());
    // Label
    Label& label = frame.labels.emplace();
    label.arity =  funcinst.type.results.size();
    // Function
    if(std::holds_alternative<WasmFunc>(funcinst.body)){
        WasmFunc& func = std::get<WasmFunc>(funcinst.body);
        for(ValueType type : func.locals){
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
        Label::Counters& pc = label.pc.emplace();
        pc.current = 0;
        pc.continuation = func.body.size() - 1;
    }
}

std::vector<Value> Stack::run(){
    RunVisitor visitor(*this);
    while(!frames.empty()){
        Frame& frame = frames.top();
        if(frame.labels.empty()){
            throw Exception::invalid_label();
        }
        Label& label = frame.labels.top();
        FuncInst& funcinst = store.funcs[frame.funcaddr];
        if(std::holds_alternative<WasmFunc>(funcinst.body)){
            if(!label.pc.has_value()){
                throw Exception::invalid_label();
            }
            WasmFunc& func = std::get<WasmFunc>(funcinst.body);
            if(label.pc->current >= func.body.size()){
                throw Exception::no_end_of_func();
            }
            std::visit(visitor, func.body[label.pc->current]);
            label.pc->current += 1;
        }else{
            hostfunc_t func = std::get<hostfunc_t>(funcinst.body);
            auto results = func(*this);
            frames.pop();
            if(frames.empty()){
                this->results = results;
            }else if(!frames.top().labels.empty()){
                frames.top().labels.top().values.insert(results);
            }else{
                throw Exception::invalid_label();
            }
        }
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
func_not_exist::func_not_exist() :
    Exception("function not exist") {}
invalid_label::invalid_label() :
    Exception("invalid label") {}