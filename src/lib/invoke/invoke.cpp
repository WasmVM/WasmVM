// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <instances/Stack.hpp>
#include "exception.hpp"
#include "RunVisitor.hpp"
#include <instances/TagInst.hpp>
#ifdef WASMVM_DEBUG
#include <Debugger.hpp>
#endif
#ifdef WASMVM_PROFILE
#include <Profiler.hpp>
#endif
#include <iterator>
#include <optional>

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
            frame.locals.emplace_back(default_value(type, frame_module.types));
        }
        Label::Counters& pc = label.pc.emplace();
        pc.current = 0;
        pc.continuation = func.body.size() - 1;
    }
#ifdef WASMVM_PROFILE
    if(observers.profiler != nullptr){
        observers.profiler->on_enter(funcaddr);
    }
#endif
}

std::vector<Value> Stack::run(){
    RunVisitor visitor(*this);
#ifdef WASMVM_PROFILE
    // One section per run(). However run() ends -- return, trap, uncaught
    // exception, interrupt -- the frames still open exit as it ends.
    struct ProfileSection {
        Profiler* profiler;
        ~ProfileSection(){
            if(profiler != nullptr){
                profiler->end_stack();
            }
        }
    } profile_section {observers.profiler};
    if(observers.profiler != nullptr){
        observers.profiler->begin_stack(*this);
    }
#endif
    while(!frames.empty()){
        if((interrupt != nullptr) && interrupt->load(std::memory_order_relaxed)){
            throw Exception::interrupted();
        }
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
#ifdef WASMVM_DEBUG
            const Debugger::Position position {frame.funcaddr, (index_t)label.pc->current};
            if(observers.debugger != nullptr){
                observers.debugger->before_instr(*this, position);
            }
#endif
#ifdef WASMVM_PROFILE
            if(observers.profiler != nullptr){
                observers.profiler->on_instr(func.body[label.pc->current].opcode);
            }
#endif
            try {
                visitor.run(func.body[label.pc->current++]);
            } catch (WasmThrow& thrown) {
                // Find the handler first, without popping anything, and unwind
                // only once the outcome is known: an uncaught throw is then
                // reported with every frame still intact.
                struct Handler {
                    size_t frame_depth; // frames above the handler's frame
                    size_t label_depth; // labels above the try_table label in it
                    Label::CatchEntry entry;
                };
                std::optional<Handler> handler;
                size_t frame_depth = 0;
                for(auto f = frames.rbegin(); f != frames.rend() && !handler; ++f, ++frame_depth){
                    size_t label_depth = 0;
                    for(auto l = f->labels.rbegin(); l != f->labels.rend() && !handler; ++l, ++label_depth){
                        for(const Label::CatchEntry& ce : l->catches){
                            if(ce.kind == Label::CatchEntry::CatchAll || ce.kind == Label::CatchEntry::CatchAllRef
                                || ce.tag_addr == thrown.exn->tag_addr){
                                handler.emplace(Handler {frame_depth, label_depth, ce});
                                break;
                            }
                        }
                    }
                }
                if(!handler){
#ifdef WASMVM_DEBUG
                    if(observers.debugger != nullptr){
                        observers.debugger->uncaught(*this, position, thrown.exn);
                    }
#endif
                    while(!frames.empty()){
#ifdef WASMVM_PROFILE
                        if(observers.profiler != nullptr){
                            observers.profiler->on_exit();
                        }
#endif
                        frames.pop();
                    }
                    throw Exception::Uncaught(thrown.exn);
                }
                for(size_t i = 0; i < handler->frame_depth; ++i){
#ifdef WASMVM_PROFILE
                    if(observers.profiler != nullptr){
                        observers.profiler->on_exit();
                    }
#endif
                    frames.pop();
                }
                Frame& f = frames.top();
                for(size_t i = 0; i < handler->label_depth; ++i){
                    f.labels.pop();
                }
                Label& lbl = f.labels.top();
                const Label::CatchEntry& ce = handler->entry;
                // Push catch values onto try_table label
                if(ce.kind == Label::CatchEntry::Catch){
                    for(auto& v : thrown.exn->values) lbl.values.emplace(v);
                } else if(ce.kind == Label::CatchEntry::CatchRef){
                    for(auto& v : thrown.exn->values) lbl.values.emplace(v);
                    lbl.values.emplace(Value(exnref_t(thrown.exn)));
                } else if(ce.kind == Label::CatchEntry::CatchAllRef){
                    lbl.values.emplace(Value(exnref_t(thrown.exn)));
                }
                // Branch to the catch target. Per spec, label_idx
                // is relative to the context enclosing the
                // try_table, so its own label pops first and then
                // label_idx more.
                std::vector<Value> values = lbl.values.get();
                for(index_t i = ce.label_idx + 1; i > 0; --i){
                    f.labels.pop();
                }
                Label& target = f.labels.top();
                if(target.arity > 0){
                    target.values.insert(std::vector<Value>(values.end() - target.arity, values.end()));
                }
                if(target.pc){
                    target.pc->current = target.pc->continuation;
                }
            }
#ifdef WASMVM_DEBUG
            catch (Exception::Trap& trap) {
                if(observers.debugger != nullptr){
                    observers.debugger->trapped(*this, position, trap);
                }
                throw;
            }
#endif
        }else{
            hostfunc_t func = std::get<hostfunc_t>(funcinst.body);
#ifdef WASMVM_DEBUG
            std::vector<Value> results;
            try {
                results = func(*this);
            } catch (Exception::Trap& trap) {
                if(observers.debugger != nullptr){
                    // A host function has no instructions: report the call
                    // instruction in the wasm frame that called it (its pc has
                    // already moved past it). An entry host function has no
                    // caller: its own funcaddr, instr (index_t)-1.
                    Debugger::Position position {frame.funcaddr, (index_t)-1};
                    auto caller = std::next(frames.rbegin());
                    if(caller != frames.rend() && !caller->labels.empty() && caller->labels.top().pc){
                        position = {caller->funcaddr, (index_t)(caller->labels.top().pc->current - 1)};
                    }
                    observers.debugger->trapped(*this, position, trap);
                }
                throw;
            }
#else
            auto results = func(*this);
#endif
#ifdef WASMVM_PROFILE
            if(observers.profiler != nullptr){
                observers.profiler->on_exit();
            }
#endif
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
    // Takes the values: the label is left empty. Every caller either pops the
    // label straight after or is a branch that puts the results back itself,
    // and a branch to depth 0 keeps the very label it read from -- copying
    // there left the old values underneath the ones inserted back, so the
    // operands came out duplicated.
    std::vector<Value> values(c.begin(), c.end());
    c.clear();
    return values;
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
    Trap("unreachable") {}
no_end_of_func::no_end_of_func() :
    Exception("function reach end without 'end' instruction") {}
func_not_exist::func_not_exist() :
    Exception("function not exist") {}
invalid_label::invalid_label() :
    Exception("invalid label") {}
invalid_elem_index::invalid_elem_index() :
    Trap("invalid index of element") {}
invalid_reference::invalid_reference() :
    Exception("invalid reference") {}
uninitialized_element::uninitialized_element() :
    Trap("uninitialized element") {}
function_type_unmatch::function_type_unmatch() :
    Trap("function type unmatch") {}
invalid_value::invalid_value() :
    Exception("invalid value") {}
length_too_long::length_too_long() :
    Trap("length too long") {}
cast_failure::cast_failure() :
    Trap("cast failure") {}
null_reference::null_reference() :
    Trap("null reference") {}
invalid_conversion_to_integer::invalid_conversion_to_integer() :
    Trap("invalid conversion to integer") {}
integer_divide_by_zero::integer_divide_by_zero() :
    Trap("integer divide by zero") {}
integer_overflow::integer_overflow() :
    Trap("integer overflow") {}

invalid_argument::~invalid_argument() = default;
invalid_results::~invalid_results() = default;
unreachable::~unreachable() = default;
no_end_of_func::~no_end_of_func() = default;
func_not_exist::~func_not_exist() = default;
invalid_label::~invalid_label() = default;
invalid_elem_index::~invalid_elem_index() = default;
invalid_reference::~invalid_reference() = default;
uninitialized_element::~uninitialized_element() = default;
invalid_value::~invalid_value() = default;
function_type_unmatch::~function_type_unmatch() = default;
length_too_long::~length_too_long() = default;
cast_failure::~cast_failure() = default;
null_reference::~null_reference() = default;
invalid_conversion_to_integer::~invalid_conversion_to_integer() = default;
integer_divide_by_zero::~integer_divide_by_zero() = default;
integer_overflow::~integer_overflow() = default;
