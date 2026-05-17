// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <structures/WasmInstr.hpp>
#include <Util.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

void RunVisitor::operator()(Instr::Throw& instr){
    Frame& frame = stack.frames.top();
    Label& label = frame.labels.top();
    // Resolve tag address
    if(instr.index >= frame.module.tagaddrs.size()){
        throw Exception::invalid_reference();
    }
    index_t tag_addr = frame.module.tagaddrs[instr.index];
    TagInst& tag = stack.store.tags[tag_addr];
    // Pop argument values from stack (in order: last param on top)
    std::vector<Value> args;
    args.resize(tag.type.params.size());
    for(int i = (int)tag.type.params.size() - 1; i >= 0; --i){
        args[i] = label.values.top();
        label.values.pop();
    }
    // Create and throw the Wasm exception
    auto exn = std::make_shared<WasmException>(tag_addr, std::move(args));
    throw WasmThrow(exn);
}

void RunVisitor::operator()(Instr::Throw_ref&){
    Label& label = stack.frames.top().labels.top();
    Value val = label.values.top();
    label.values.pop();
    if(!std::holds_alternative<exnref_t>(val)){
        throw Exception::invalid_reference();
    }
    exnref_t exnref = std::get<exnref_t>(val);
    if(!exnref.has_value()){
        throw Exception::null_reference();
    }
    throw WasmThrow(exnref.value);
}

void RunVisitor::operator()(Instr::Try_table& instr){
    Frame& frame = stack.frames.top();
    Label& label = frame.labels.top();
    WasmFunc& func = std::get<WasmFunc>(stack.store.funcs[frame.funcaddr].body);
    // Get type
    FuncType type;
    if(instr.type){
        type = frame.module.types[instr.type.value()];
    }
    // Pop parameters
    std::vector<Value> args;
    if(type.params.size() > 0){
        args.resize(type.params.size());
        for(int i = type.params.size() - 1; i >= 0; --i){
            args[i] = label.values.top();
            label.values.pop();
        }
    }
    // Create new label (like Block)
    Label& new_label = frame.labels.emplace();
    new_label.arity = type.results.size();
    new_label.values.insert(args);
    new_label.pc.emplace();
    new_label.pc->current = label.pc->current;
    new_label.pc->continuation = label.pc->current;
    int level = 0;
    while(new_label.pc->continuation < func.body.size()){
        switch(func.body[new_label.pc->continuation].opcode) {
            case Opcode::Block:
            case Opcode::Loop:
            case Opcode::If:
            case Opcode::Try_table:
                level += 1; break;
            case Opcode::End:
                level -= 1; break;
            default: break;
        }
        if(level >= 0){
            new_label.pc->continuation += 1;
        }else{
            break;
        }
    }
    if(new_label.pc->continuation >= func.body.size() - 1){
        throw Exception::invalid_label();
    }
    // Resolve catch entries
    for(const Instr::TryCatchEntry& catch_instr : instr.catches){
        Label::CatchEntry& entry = new_label.catches.emplace_back();
        entry.kind = (Label::CatchEntry::Kind)catch_instr.kind;
        entry.label_idx = catch_instr.label_idx;
        if(catch_instr.kind == 0 || catch_instr.kind == 1){
            // Resolve tag index to tag address
            if(catch_instr.tag_idx >= frame.module.tagaddrs.size()){
                throw Exception::invalid_reference();
            }
            entry.tag_addr = frame.module.tagaddrs[catch_instr.tag_idx];
        } else {
            entry.tag_addr = 0;  // unused
        }
    }
}
