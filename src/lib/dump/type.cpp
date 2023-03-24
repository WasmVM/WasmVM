// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, ValueType& type){
    switch(type){
        case ValueType::i32:
            stream << "i32";
        break;
        case ValueType::i64:
            stream << "i64";
        break;
        case ValueType::f32:
            stream << "f32";
        break;
        case ValueType::f64:
            stream << "f64";
        break;
        case ValueType::funcref:
            stream << "funcref";
        break;
        case ValueType::externref:
            stream << "externref";
        break;
    }
    return stream;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, FuncType& type){
    stream << "  (type (func";
    if(!type.params.empty()){
        stream << " (param";
        for(auto& param : type.params){
            stream << " " << param;
        }
        stream << ")";
    }
    if(!type.results.empty()){
        stream << " (result";
        for(auto& result : type.results){
            stream << " " << result;
        }
        stream << ")";
    }
    stream << " ) )" << std::endl;
    return stream;
}

std::ostream& WasmVM::operator<<(std::ostream& stream, TableType& table){
    stream << "  (table " << table.limits.min;
    if(table.limits.max.has_value()){
        stream << " " << table.limits.max.value();
    }
    switch(table.reftype){
        case RefType::funcref :
            stream << " funcref";
        break;
        case RefType::externref :
            stream << " externref";
        break;
    }
    stream << ")" << std::endl;
    return stream;
}