// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <variant>
#include "Util.hpp"

using namespace WasmVM;

template<> void Validate::Validator::operator()<WasmExport>(const WasmExport& export_){
    switch(export_.desc){
        case WasmExport::DescType::func :
            if(export_.index >= context.funcs.size()){
                throw Exception::Exception("function index not found");
            }
        break;
        case WasmExport::DescType::table :
            if(export_.index >= context.tables.size()){
                throw Exception::Exception("table index not found");
            }
        break;
        case WasmExport::DescType::mem :
            if(export_.index >= context.mems.size()){
                throw Exception::Exception("memory index not found");
            }
        break;
        case WasmExport::DescType::global :
            if(export_.index >= context.globals.size()){
                throw Exception::Exception("global index not found");
            }
        break;
    }
}