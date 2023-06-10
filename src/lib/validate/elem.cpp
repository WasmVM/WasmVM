// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <variant>
#include "Util.hpp"

using namespace WasmVM;

template<> void Validate::Validator::operator()<WasmElem>(const WasmElem& elem){
    for(const ConstInstr& instr : elem.elemlist){
        std::visit(overloaded {
            [&](const Instr::Ref_null& ins){
                if(ins.heaptype != elem.type){
                    throw Exception::Exception("reference type mismatch");
                }
            },
            [&](const Instr::Ref_func& ins){
                if((ValueType)elem.type != ValueType::funcref){
                    throw Exception::Exception("reference type mismatch");
                }
                if(ins.index >= context.funcs.size()){
                    throw Exception::Exception("func index not found in ref.func");
                }
            },
            [&](const Instr::Global_get& ins){
                if(ins.index >= context.globals.size()){
                    throw Exception::Exception("global index not found in global.get");
                }
                const GlobalType globaltype = context.globals[ins.index];
                if(globaltype.type != (ValueType)elem.type){
                    throw Exception::Exception("global value type mismatch");
                }
                if(globaltype.mut != GlobalType::constant){
                    throw Exception::Exception("elem should be initailized by constant global");
                }
            },
            [](auto&){
                throw Exception::Exception("only reference types can be initialized by element segment");
            }
        }, instr);
    }
    if(elem.mode.type == WasmElem::ElemMode::Mode::active){
        if(context.tables.empty() || (elem.mode.tableidx && (elem.mode.tableidx.value() >= context.tables.size()))){
            throw Exception::Exception("table index not found");
        }
        if(elem.mode.offset){
            if(std::holds_alternative<Instr::Global_get>(elem.mode.offset.value())){
                const Instr::Global_get ins = std::get<Instr::Global_get>(elem.mode.offset.value());
                if(ins.index >= context.globals.size()){
                    throw Exception::Exception("global index not found in global.get");
                }
                const GlobalType& globaltype = context.globals[ins.index];
                if(globaltype.type != ValueType::i32 && globaltype.type != ValueType::i64){
                    throw Exception::Exception("offset should be i32 or i64");
                }
            }else if(!std::holds_alternative<Instr::I32_const>(elem.mode.offset.value()) && !std::holds_alternative<Instr::I64_const>(elem.mode.offset.value())){
                throw Exception::Exception("offset should be i32 or i64");
            }
        }
    }
}