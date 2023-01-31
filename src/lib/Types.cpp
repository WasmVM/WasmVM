// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <Types.hpp>

using namespace WasmVM;

bool FuncType::operator==(const FuncType& op){
    // param
    if(op.params.size() != params.size()){
        return false;
    }
    for(size_t i = 0; i < params.size(); ++i){
        if(params[i] != op.params[i]){
            return false;
        }
    }
    // result
    if(op.results.size() != results.size()){
        return false;
    }
    for(size_t i = 0; i < results.size(); ++i){
        if(results[i] != op.results[i]){
            return false;
        }
    }
    return true;
}