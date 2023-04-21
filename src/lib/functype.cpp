// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>
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