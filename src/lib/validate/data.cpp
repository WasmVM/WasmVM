// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <variant>
#include "Util.hpp"

using namespace WasmVM;

template<> void Validate::Validator::operator()<WasmData>(const WasmData& data){
    if(data.mode.type == WasmData::DataMode::Mode::active){
        if(context.mems.empty() || (data.mode.memidx && (data.mode.memidx.value() >= context.mems.size()))){
            throw Exception::Exception("memory index not found");
        }
        if(data.mode.offset){
            if(std::holds_alternative<Instr::Global_get>(data.mode.offset.value())){
                const Instr::Global_get ins = std::get<Instr::Global_get>(data.mode.offset.value());
                if(ins.index >= context.globals.size()){
                    throw Exception::Exception("global index not found in global.get");
                }
                const GlobalType& globaltype = context.globals[ins.index];
                if(globaltype.type != ValueType::i32 && globaltype.type != ValueType::i64){
                    throw Exception::Exception("offset should be i32 or i64");
                }
            }else if(!std::holds_alternative<Instr::I32_const>(data.mode.offset.value()) && !std::holds_alternative<Instr::I64_const>(data.mode.offset.value())){
                throw Exception::Exception("offset should be i32 or i64");
            }
        }
    }
}