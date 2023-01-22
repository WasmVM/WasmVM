/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.hpp>

#include <string>
#include <sstream>
#include <utility>

using namespace WasmVM;

static std::ostream& operator<<(std::ostream& stream, ValueType& type){
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

static std::ostream& operator<<(std::ostream& stream, FuncType& type){
    stream << "  (type " << (type.id.empty() ? "" : type.id + " ") << "(func ";
    for(auto& param : type.params){
        stream << "(param " << (param.first.empty() ? "" : param.first + " ") << param.second << ") ";
    }
    for(auto& result : type.results){
        stream << "(result " << result << ") ";
    }
    stream << ") )" << std::endl;
    return stream;
}

static std::ostream& operator<<(std::ostream& stream, WasmModule& module){
    stream << "(module " << module.id << std::endl;
    for(FuncType& type : module.types){
        stream << type;
    }
    stream << ")" << std::endl;
    return stream;
}

std::string WasmVM::module_dump(WasmModule& module){
    std::stringstream stream;
    stream << module;
    return stream.str();
}