// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>
#include <Util.hpp>
#include <exception.hpp>
#include <string>

using namespace WasmVM;

bool WasmVM::value_matches(const Value& value, const ValueType& type){
    switch(type.kind){
        case ValueType::i32:      return std::holds_alternative<i32_t>(value);
        case ValueType::i64:      return std::holds_alternative<i64_t>(value);
        case ValueType::f32:      return std::holds_alternative<f32_t>(value);
        case ValueType::f64:      return std::holds_alternative<f64_t>(value);
        case ValueType::funcref:
        case ValueType::nullfuncref:   return std::holds_alternative<funcref_t>(value);
        case ValueType::externref:
        case ValueType::nullexternref: return std::holds_alternative<externref_t>(value);
        case ValueType::i31ref:   return std::holds_alternative<i31ref_t>(value);
        case ValueType::exnref:   return std::holds_alternative<exnref_t>(value);
        case ValueType::structref: return std::holds_alternative<structref_t>(value);
        case ValueType::arrayref:  return std::holds_alternative<arrayref_t>(value);
        // Several representations inhabit these, and a concrete reference's
        // type index is checked statically, not here.
        case ValueType::anyref:
        case ValueType::eqref:
        case ValueType::nullref:
        case ValueType::ref:
        break;
    }
    return true;
}

std::size_t std::hash<WasmVM::ValueType>::operator()(const WasmVM::ValueType& type) const {
    std::size_t value = (std::size_t)type.kind;
    if(type.is_ref()){
        value = (value << 1) | (type.nullable ? 1 : 0);
    }
    if(type.is_concrete()){
        value = (value << 32) ^ (std::size_t)type.typeidx;
    }
    return value;
};

static void hash_valtypes(std::string& signature, const std::vector<WasmVM::ValueType>& types){
    std::hash<WasmVM::ValueType> hashfunc;
    for(const WasmVM::ValueType& type : types){
        signature += std::to_string(hashfunc(type));
        signature += ',';
    }
}

std::size_t std::hash<WasmVM::FuncType>::operator()(const WasmVM::FuncType& functype) const {
    std::string signature;
    hash_valtypes(signature, functype.params);
    signature += ':';
    hash_valtypes(signature, functype.results);
    return std::hash<std::string>{}(signature);
};

bool WasmVM::operator==(const WasmVM::FuncType& a, const WasmVM::FuncType& b){
    std::hash<WasmVM::FuncType> hashfunc;
    return hashfunc(a) == hashfunc(b);
}

bool WasmVM::operator==(const FieldType& a, const FieldType& b){
    return (a.mut == b.mut) && (a.storage == b.storage);
}
bool WasmVM::operator==(const StructType& a, const StructType& b){
    return a.fields == b.fields;
}
bool WasmVM::operator==(const ArrayType& a, const ArrayType& b){
    return a.field == b.field;
}
bool WasmVM::operator==(const RecGroup& a, const RecGroup& b){
    return (a.first == b.first) && (a.count == b.count);
}
bool WasmVM::operator==(const SubType& a, const SubType& b){
    return (a.final == b.final) && (a.supers == b.supers) && (a.comp == b.comp);
}

const FuncType& SubType::func() const {
    if(!is_func()){
        throw Exception::Exception("type index is not a function type");
    }
    return std::get<FuncType>(comp);
}
FuncType& SubType::func(){
    if(!is_func()){
        throw Exception::Exception("type index is not a function type");
    }
    return std::get<FuncType>(comp);
}

std::size_t std::hash<WasmVM::SubType>::operator()(const WasmVM::SubType& type) const {
    std::string signature;
    signature += type.final ? 'F' : 'f';
    for(index_t super : type.supers){
        signature += std::to_string(super);
        signature += ',';
    }
    signature += ':';
    auto field = [&](const FieldType& f){
        signature += f.mut ? 'm' : 'c';
        std::visit(overloaded {
            [&](ValueType v){ signature += std::to_string(std::hash<ValueType>{}(v)); },
            [&](PackedType p){ signature += (p == PackedType::i8) ? 'b' : 'h'; }
        }, f.storage);
        signature += '.';
    };
    std::visit(overloaded {
        [&](const FuncType& f){ signature += 'F'; signature += std::to_string(std::hash<FuncType>{}(f)); },
        [&](const StructType& s){ signature += 'S'; for(const FieldType& f : s.fields) field(f); },
        [&](const ArrayType& a){ signature += 'A'; field(a.field); }
    }, type.comp);
    return std::hash<std::string>{}(signature);
}