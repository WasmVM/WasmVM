// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>
#include <Util.hpp>
#include <exception.hpp>
#include <string>

using namespace WasmVM;

std::size_t std::hash<WasmVM::FuncType>::operator()(const WasmVM::FuncType& functype) const {
    std::string signature;
    for(auto param : functype.params){
        signature += (char) param + '0';
    }
    signature += ':';
    for(auto result : functype.results){
        signature += (char) result + '0';
    }
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
            [&](ValueType v){ signature += (char)('0' + v); },
            [&](PackedType p){ signature += (p == PackedType::i8) ? 'b' : 'h'; }
        }, f.storage);
    };
    std::visit(overloaded {
        [&](const FuncType& f){ signature += 'F'; signature += std::to_string(std::hash<FuncType>{}(f)); },
        [&](const StructType& s){ signature += 'S'; for(const FieldType& f : s.fields) field(f); },
        [&](const ArrayType& a){ signature += 'A'; field(a.field); }
    }, type.comp);
    return std::hash<std::string>{}(signature);
}