// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <variant>
#include "Util.hpp"

using namespace WasmVM;

void Validate::Validator::operator()(const WasmGlobal& global, index_t idx){
    std::visit(overloaded {
        [&](const Instr::Ref_null& ins){
            if((global.type.type != ValueType::funcref) && (global.type.type != ValueType::externref)){
                throw Exception::Exception("invalid type if ref.null");
            }
            if((ValueType)ins.heaptype != global.type.type){
                throw Exception::Exception("value type mismatch");
            }
        },
        [&](const Instr::Ref_func& ins){
            if((ValueType)global.type.type != ValueType::funcref){
                throw Exception::Exception("value type mismatch");
            }
            if(ins.index >= context.funcs.size()){
                throw Exception::Exception("func index not found in ref.func");
            }
        },
        [&](const Instr::Global_get& ins){
            if(ins.index >= context.globals.size()){
                throw Exception::Exception("global index not found in global.get");
            }
            if(ins.index == idx){
                throw Exception::Exception("global index can't be self in global.get");
            }
            const GlobalType& globaltype = context.globals[ins.index].get();
            if(global.type.type != globaltype.type){
                throw Exception::Exception("value type mismatch");
            }
            if(globaltype.mut != GlobalType::constant){
                throw Exception::Exception("global should be initailized by constant global");
            }
        },
        [&](const Instr::I32_const&){
            if(global.type.type != ValueType::i32){
                throw Exception::Exception("value type mismatch");
            }
        },
        [&](const Instr::I64_const&){
            if(global.type.type != ValueType::i64){
                throw Exception::Exception("value type mismatch");
            }
        },
        [&](const Instr::F32_const&){
            if(global.type.type != ValueType::f32){
                throw Exception::Exception("value type mismatch");
            }
        },
        [&](const Instr::F64_const&){
            if(global.type.type != ValueType::f64){
                throw Exception::Exception("value type mismatch");
            }
        }
    }, global.init);
}