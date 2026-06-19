// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <instances/Stack.hpp>
#include "exception.hpp"
#include "RunVisitor.hpp"
#include <instances/TagInst.hpp>

using namespace WasmVM;

void Stack::invoke(index_t funcaddr, std::vector<Value> args){
    // Check func type
    FuncInst& funcinst = store.funcs[funcaddr];
    // Frame. A host function (e.g. sys_fs.write) carries no linear memory of
    // its own — a syscall reads/writes its *caller's* memory — so its frame
    // borrows the caller's module instance. WebAssembly functions use their
    // own module as usual. The very first call has no caller; host entry
    // points don't touch wasm memory, so funcinst.module is fine there.
    const bool is_host = !std::holds_alternative<WasmFunc>(funcinst.body);
    ModuleInst& frame_module = (is_host && !frames.empty())
                                   ? frames.top().module
                                   : funcinst.module;
    Frame& frame = frames.emplace(frame_module, funcaddr);
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
                case ValueType::i31ref :
                    frame.locals.emplace_back(i31ref_t());
                break;
                case ValueType::exnref :
                    frame.locals.emplace_back(exnref_t());
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
            try {
                visitor.run(func.body[label.pc->current++]);
            } catch (WasmThrow& thrown) {
                bool handled = false;
                while(!frames.empty() && !handled){
                    Frame& f = frames.top();
                    while(!f.labels.empty() && !handled){
                        Label& lbl = f.labels.top();
                        for(auto& ce : lbl.catches){
                            bool matches = false;
                            if(ce.kind == Label::CatchEntry::CatchAll || ce.kind == Label::CatchEntry::CatchAllRef){
                                matches = true;
                            } else if(ce.tag_addr == thrown.exn->tag_addr){
                                matches = true;
                            }
                            if(matches){
                                // Push catch values onto try_table label
                                if(ce.kind == Label::CatchEntry::Catch){
                                    for(auto& v : thrown.exn->values) lbl.values.emplace(v);
                                } else if(ce.kind == Label::CatchEntry::CatchRef){
                                    for(auto& v : thrown.exn->values) lbl.values.emplace(v);
                                    lbl.values.emplace(Value(exnref_t(thrown.exn)));
                                } else if(ce.kind == Label::CatchEntry::CatchAllRef){
                                    lbl.values.emplace(Value(exnref_t(thrown.exn)));
                                }
                                // Execute br(label_idx) from try_table label position
                                std::vector<Value> values = lbl.values.get();
                                for(index_t i = ce.label_idx; i > 0; --i){
                                    f.labels.pop();
                                }
                                Label& target = f.labels.top();
                                if(target.arity > 0){
                                    target.values.insert(std::vector<Value>(values.end() - target.arity, values.end()));
                                }
                                if(target.pc){
                                    target.pc->current = target.pc->continuation;
                                }
                                handled = true;
                                break;
                            }
                        }
                        if(!handled){
                            f.labels.pop();
                        }
                    }
                    if(!handled){
                        frames.pop();
                    }
                }
                if(!handled){
                    throw Exception::Exception("unhandled WebAssembly exception");
                }
            }
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
invalid_elem_index::invalid_elem_index() :
    Exception("invalid index of element") {}
invalid_reference::invalid_reference() :
    Exception("invalid reference") {}
function_type_unmatch::function_type_unmatch() :
    Exception("function type unmatch") {}
invalid_value::invalid_value() :
    Exception("invalid value") {}
length_too_long::length_too_long() :
    Exception("length too long") {}
cast_failure::cast_failure() :
    Exception("cast failure") {}
null_reference::null_reference() :
    Exception("null reference") {}
invalid_conversion_to_integer::invalid_conversion_to_integer() :
    Exception("invalid conversion to integer") {}