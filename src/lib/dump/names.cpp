// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include "../parse/Tokenizer.hpp"

#include <set>

using namespace WasmVM;

static thread_local DumpNames* current_names = nullptr;

// The printable part of one index space: valid ids, the first of equal ones.
static WasmNames::NameMap printable(const WasmNames::NameMap& map, index_t from = 0){
    WasmNames::NameMap result;
    std::set<std::string> seen;
    for(auto& [idx, name] : map){
        if(idx < from || !Parse::is_id_name(name) || seen.contains(name)){
            continue;
        }
        seen.insert(name);
        result[idx] = name;
    }
    return result;
}

static WasmNames::IndirectNameMap printable(const WasmNames::IndirectNameMap& map){
    WasmNames::IndirectNameMap result;
    for(auto& [idx, inner] : map){
        WasmNames::NameMap kept = printable(inner);
        if(!kept.empty()){
            result[idx] = kept;
        }
    }
    return result;
}

static const std::string* find(const WasmNames::NameMap& map, index_t idx){
    auto found = map.find(idx);
    return (found != map.end()) ? &found->second : nullptr;
}

static const std::string* find(const WasmNames::IndirectNameMap& map, index_t outer, index_t inner){
    auto found = map.find(outer);
    return (found != map.end()) ? find(found->second, inner) : nullptr;
}

DumpNames::DumpNames(const WasmModule& module){
    const WasmNames& source = module.names;
    if(source.module && Parse::is_id_name(source.module.value())){
        names.module = source.module;
    }
    names.funcs = printable(source.funcs);
    names.tables = printable(source.tables);
    names.mems = printable(source.mems);
    names.globals = printable(source.globals);
    names.elems = printable(source.elems);
    names.datas = printable(source.datas);
    names.tags = printable(source.tags);
    names.types = printable(source.types);
    names.fields = printable(source.fields);
    names.type_params = printable(source.type_params);
    names.labels = printable(source.labels);

    // Parameter counts by function index; imports have no locals to print.
    for(const WasmImport& import : module.imports){
        if(std::holds_alternative<index_t>(import.desc)){
            param_counts.emplace_back(index_npos);
        }
    }
    for(const WasmFunc& func : module.funcs){
        index_t count = 0;
        if(func.typeidx < module.types.size() && module.types[func.typeidx].is_func()){
            count = module.types[func.typeidx].func().params.size();
        }
        param_counts.emplace_back(count);
    }
    for(auto& [idx, inner] : source.locals){
        if(idx >= param_counts.size() || param_counts[idx] == index_npos){
            continue;
        }
        WasmNames::NameMap kept = printable(inner, param_counts[idx]);
        if(!kept.empty()){
            names.locals[idx] = kept;
        }
    }
}

DumpNames::Scope::Scope(DumpNames& names) : previous(current_names){
    current_names = &names;
}
DumpNames::Scope::~Scope(){
    current_names = previous;
}
DumpNames* DumpNames::current(){
    return current_names;
}

const std::string* DumpNames::module() const { return names.module ? &names.module.value() : nullptr; }
const std::string* DumpNames::func(index_t idx) const { return find(names.funcs, idx); }
const std::string* DumpNames::table(index_t idx) const { return find(names.tables, idx); }
const std::string* DumpNames::mem(index_t idx) const { return find(names.mems, idx); }
const std::string* DumpNames::global(index_t idx) const { return find(names.globals, idx); }
const std::string* DumpNames::elem(index_t idx) const { return find(names.elems, idx); }
const std::string* DumpNames::data(index_t idx) const { return find(names.datas, idx); }
const std::string* DumpNames::tag(index_t idx) const { return find(names.tags, idx); }
const std::string* DumpNames::type(index_t idx) const { return find(names.types, idx); }
const std::string* DumpNames::field(index_t typeidx, index_t fieldidx) const { return find(names.fields, typeidx, fieldidx); }
const std::string* DumpNames::type_param(index_t typeidx, index_t paramidx) const { return find(names.type_params, typeidx, paramidx); }

const std::string* DumpNames::local(index_t idx) const {
    return func_idx ? find(names.locals, func_idx.value(), idx) : nullptr;
}

const std::string* DumpNames::label(index_t depth) const {
    if(!func_idx || depth >= open_labels.size()){
        return nullptr; // the function body's own label has no name
    }
    return find(names.labels, func_idx.value(), open_labels[open_labels.size() - 1 - depth]);
}

index_t DumpNames::param_count(index_t funcidx) const {
    if(funcidx >= param_counts.size() || param_counts[funcidx] == index_npos){
        return 0;
    }
    return param_counts[funcidx];
}

void DumpNames::begin_func(index_t funcidx){
    func_idx = funcidx;
    label_count = 0;
    open_labels.clear();
}

void DumpNames::end_func(){
    func_idx.reset();
    open_labels.clear();
}

const std::string* DumpNames::open_label(){
    index_t idx = label_count++;
    open_labels.push_back(idx);
    return func_idx ? find(names.labels, func_idx.value(), idx) : nullptr;
}

void DumpNames::close_label(){
    if(!open_labels.empty()){
        open_labels.pop_back();
    }
}

std::ostream& WasmVM::dump_index(std::ostream& stream, const std::string* name, index_t idx){
    if(name){
        return stream << "$" << *name;
    }
    return stream << idx;
}

std::ostream& WasmVM::dump_def_name(std::ostream& stream, const std::string* name){
    if(name){
        stream << " $" << *name;
    }
    return stream;
}

const std::string* DumpNames::lookup(DumpSpace space, index_t idx) const {
    switch(space){
        case DumpSpace::func:   return func(idx);
        case DumpSpace::table:  return table(idx);
        case DumpSpace::mem:    return mem(idx);
        case DumpSpace::global: return global(idx);
        case DumpSpace::elem:   return elem(idx);
        case DumpSpace::data:   return data(idx);
        case DumpSpace::tag:    return tag(idx);
        case DumpSpace::type:   return type(idx);
        case DumpSpace::local:  return local(idx);
        case DumpSpace::label:  return label(idx);
    }
    return nullptr;
}

const std::string* DumpNames::define(DumpSpace space, index_t idx){
    defined.emplace(space, idx);
    return lookup(space, idx);
}

const std::string* DumpNames::reference(DumpSpace space, index_t idx) const {
    // Locals are defined in the function's header and labels by the open
    // blocks, both before any reference to them.
    if(space != DumpSpace::local && space != DumpSpace::label && !defined.contains({space, idx})){
        return nullptr;
    }
    return lookup(space, idx);
}

std::ostream& WasmVM::dump_ref(std::ostream& stream, DumpSpace space, index_t idx){
    const DumpNames* names = DumpNames::current();
    return dump_index(stream, names ? names->reference(space, idx) : nullptr, idx);
}

std::ostream& WasmVM::dump_field_ref(std::ostream& stream, index_t typeidx, index_t fieldidx){
    const DumpNames* names = DumpNames::current();
    return dump_index(stream, names ? names->field(typeidx, fieldidx) : nullptr, fieldidx);
}
