// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <vector>
#include <cassert>
#include <sstream>

using namespace WasmVM;

static inline bool is_unknown(Validate::State::StackValue value){
    return std::holds_alternative<std::monostate>(value);
}

static bool is_num(Validate::State::StackValue value){
    if(is_unknown(value)){
        return true;
    }
    ValueType& type = std::get<ValueType>(value);
    return (type == ValueType::i32) || (type == ValueType::i64) || (type == ValueType::f32) || (type == ValueType::f64);
}

static bool is_ref(Validate::State::StackValue value){
    if(is_unknown(value)){
        return true;
    }
    ValueType& type = std::get<ValueType>(value);
    return (type == ValueType::funcref) || (type == ValueType::externref);
}

template<> void Validate::Validator::operator()<WasmFunc>(const WasmFunc& func){
    // type
    assert(func.typeidx < context.types.size());
    FuncType func_type = context.types[func.typeidx];
    // state (context C')
    State state;
    state.locals.insert(state.locals.end(), func_type.params.begin(), func_type.params.end());
    state.locals.insert(state.locals.end(), func.locals.begin(), func.locals.end());
    state.push(Opcode::Nop, func_type);
    // body
    std::stringstream msg;
    size_t instridx = 1;
    for(const WasmInstr& instr : func.body){
        Opcode::opcode_t opcode = std::visit(overloaded {[](auto& ins){return ins.opcode;}}, instr);
        try{
            switch(opcode){
                // [] -> []
                case Opcode::Elem_drop : {
                    const Instr::Elem_drop& ins = std::get<Instr::Elem_drop>(instr);
                    if(ins.index >= context.elems.size()){
                        throw Exception::Exception("elem index not found in elem.drop");
                    }
                }break;
                case Opcode::Data_drop : {
                    const Instr::Data_drop& ins = std::get<Instr::Data_drop>(instr);
                    if(ins.index >= context.datas.size()){
                        throw Exception::Exception("data index not found in data.drop");
                    }
                }break;
                // [] -> [i32]
                case Opcode::I32_const :
                    state.push(ValueType::i32);
                break;
                case Opcode::Memory_size : {
                    const Instr::Memory_size& ins = std::get<Instr::Memory_size>(instr);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.size");
                    }
                    state.push(ValueType::i32);
                }break;
                case Opcode::Table_size : {
                    const Instr::Table_size& ins = std::get<Instr::Table_size>(instr);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.size");
                    }
                    state.push(ValueType::i32);
                }break;
                // [] -> [i64]
                case Opcode::I64_const :
                    state.push(ValueType::i64);
                break;
                // [] -> [f32]
                case Opcode::F32_const :
                    state.push(ValueType::f32);
                break;
                // [] -> [f64]
                case Opcode::F64_const :
                    state.push(ValueType::f64);
                break;
                // [] -> [funcref]
                case Opcode::Ref_func : {
                    const Instr::Ref_func& ins = std::get<Instr::Ref_func>(instr);
                    if(ins.index >= context.funcs.size()){
                        throw Exception::Exception("func index not found in ref.func");
                    }
                    state.push(ValueType::funcref);
                }break;
                // [] -> [t]
                case Opcode::Ref_null : {
                    const Instr::Ref_null& ins = std::get<Instr::Ref_null>(instr);
                    state.push((ValueType)ins.heaptype);
                }break;
                case Opcode::Local_get : {
                    const Instr::Local_get& ins = std::get<Instr::Local_get>(instr);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.get");
                    }
                    state.push(state.locals[ins.index]);
                }break;
                case Opcode::Global_get : {
                    const Instr::Global_get& ins = std::get<Instr::Global_get>(instr);
                    if(ins.index >= context.globals.size()){
                        throw Exception::Exception("global index not found in global.get");
                    }
                    state.push(context.globals[ins.index].get().type);
                }break;
                // [i32] -> [i32]
                case Opcode::Memory_grow : {
                    const Instr::Memory_grow& ins = std::get<Instr::Memory_grow>(instr);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.grow");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                // [i32] -> [t]
                case Opcode::Table_get : {
                    const Instr::Table_get& ins = std::get<Instr::Table_get>(instr);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.get");
                    }
                    const TableType& type = context.tables[ins.index].get();
                    state.pop(ValueType::i32);
                    state.push((ValueType)type.reftype);
                }break;
                case Opcode::I32_load : {
                    const Instr::I32_load& ins = std::get<Instr::I32_load>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load8_s : {
                    const Instr::I32_load8_s& ins = std::get<Instr::I32_load8_s>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load8_u : {
                    const Instr::I32_load8_u& ins = std::get<Instr::I32_load8_u>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load16_s : {
                    const Instr::I32_load16_s& ins = std::get<Instr::I32_load16_s>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load16_u : {
                    const Instr::I32_load16_u& ins = std::get<Instr::I32_load16_u>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I64_load : {
                    const Instr::I64_load& ins = std::get<Instr::I64_load>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-3");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load8_s : {
                    const Instr::I64_load8_s& ins = std::get<Instr::I64_load8_s>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load8_u : {
                    const Instr::I64_load8_u& ins = std::get<Instr::I64_load8_u>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load16_s : {
                    const Instr::I64_load16_s& ins = std::get<Instr::I64_load16_s>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load16_u : {
                    const Instr::I64_load16_u& ins = std::get<Instr::I64_load16_u>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load32_s : {
                    const Instr::I64_load32_s& ins = std::get<Instr::I64_load32_s>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load32_u : {
                    const Instr::I64_load32_u& ins = std::get<Instr::I64_load32_u>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::F32_load : {
                    const Instr::F32_load& ins = std::get<Instr::F32_load>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::f32);
                }break;
                case Opcode::F64_load : {
                    const Instr::F64_load& ins = std::get<Instr::F64_load>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-3");
                    }
                    state.pop(ValueType::i32);
                    state.push(ValueType::f64);
                }break;
                // [i32 t] -> []
                case Opcode::Table_set : {
                    const Instr::Table_set& ins = std::get<Instr::Table_set>(instr);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.set");
                    }
                    const TableType& type = context.tables[ins.index].get();
                    state.pop(std::vector<ValueType> {ValueType::i32, (ValueType)type.reftype});
                }break;
                case Opcode::I32_store : {
                    const Instr::I32_store& ins = std::get<Instr::I32_store>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I32_store8 : {
                    const Instr::I32_store8& ins = std::get<Instr::I32_store8>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I32_store16 : {
                    const Instr::I32_store16& ins = std::get<Instr::I32_store16>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I64_store : {
                    const Instr::I64_store& ins = std::get<Instr::I64_store>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I64_store8 : {
                    const Instr::I64_store8& ins = std::get<Instr::I64_store8>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I64_store16 : {
                    const Instr::I64_store16& ins = std::get<Instr::I64_store16>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::I64_store32 : {
                    const Instr::I64_store32& ins = std::get<Instr::I64_store32>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::F32_store : {
                    const Instr::F32_store& ins = std::get<Instr::F32_store>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::f32);
                    state.pop(ValueType::i32);
                }break;
                case Opcode::F64_store : {
                    const Instr::F64_store& ins = std::get<Instr::F64_store>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::f64);
                    state.pop(ValueType::i32);
                }break;
                // [t] -> []
                case Opcode::Drop :
                    state.pop<State::StackValue>();
                break;
                case Opcode::Local_set : {
                    const Instr::Local_set& ins = std::get<Instr::Local_set>(instr);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.set");
                    }
                    state.pop(state.locals[ins.index]);
                }break;
                case Opcode::Global_set : {
                    const Instr::Global_set& ins = std::get<Instr::Global_set>(instr);
                    if(ins.index >= context.globals.size()){
                        throw Exception::Exception("global index not found in global.set");
                    }
                    state.pop(context.globals[ins.index].get().type);
                }break;
                // [t*] -> []
                case Opcode::End : {
                    State::CtrlFrame ctrl = state.pop<State::CtrlFrame>();
                    state.push(ctrl.type.results);
                }break;
                // [t] -> [i32]
                case Opcode::Ref_is_null :
                    if(!is_ref(state.pop<State::StackValue>())){
                        throw Exception::Exception("ref.is_null should consume reference type");
                    }
                    state.push(ValueType::i32);
                break;
                // [t] -> [t]
                case Opcode::Local_tee : {
                    const Instr::Local_tee& ins = std::get<Instr::Local_tee>(instr);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.tee");
                    }
                    state.push(state.pop(state.locals[ins.index]));
                }break;
                // [t i32] -> [i32]
                case Opcode::Table_grow : {
                    const Instr::Table_grow& ins = std::get<Instr::Table_grow>(instr);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.grow");
                    }
                    const TableType& type = context.tables[ins.index].get();
                    state.pop(std::vector<ValueType> {(ValueType)type.reftype, ValueType::i32});
                    state.push(ValueType::i32);
                }break;
                // [t* i32] -> [t*]
                case Opcode::Br_if : {
                    state.pop(ValueType::i32);
                    const Instr::Br_if& ins = std::get<Instr::Br_if>(instr);
                    if(ins.index >= state.ctrls.size()){
                        throw Exception::Exception("label index not found in br_if");
                    }
                    std::vector<ValueType> label_type = state.ctrls[state.ctrls.size() - 1 - ins.index].types();
                    state.pop(label_type);
                    state.push(label_type);
                }break;
                // [i32 i32 i32] -> []
                case Opcode::Table_copy : {
                    const Instr::Table_copy& ins = std::get<Instr::Table_copy>(instr);
                    if(ins.dstidx >= context.tables.size()){
                        throw Exception::Exception("table destination index not found in table.copy");
                    }
                    if(ins.srcidx >= context.tables.size()){
                        throw Exception::Exception("table source index not found in table.copy");
                    }
                    if(context.tables[ins.dstidx].get().reftype != context.tables[ins.srcidx].get().reftype){
                        throw Exception::Exception("table types not match in table.copy");
                    };
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                case Opcode::Table_init : {
                    const Instr::Table_init& ins = std::get<Instr::Table_init>(instr);
                    if(ins.tableidx >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.init");
                    }
                    if(ins.elemidx >= context.elems.size()){
                        throw Exception::Exception("elem index not found in table.init");
                    }
                    if(context.tables[ins.tableidx].get().reftype != context.elems[ins.elemidx].type){
                        throw Exception::Exception("table & elem types not match in table.init");
                    };
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                case Opcode::Memory_fill : {
                    const Instr::Memory_fill& ins = std::get<Instr::Memory_fill>(instr);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.fill");
                    }
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                case Opcode::Memory_copy : {
                    const Instr::Memory_copy& ins = std::get<Instr::Memory_copy>(instr);
                    if(ins.dstidx >= context.mems.size()){
                        throw Exception::Exception("memory destination index not found in memory.copy");
                    }
                    if(ins.srcidx >= context.mems.size()){
                        throw Exception::Exception("memory source index not found in memory.copy");
                    }
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                case Opcode::Memory_init : {
                    const Instr::Memory_init& ins = std::get<Instr::Memory_init>(instr);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.init");
                    }
                    if(ins.dataidx >= context.datas.size()){
                        throw Exception::Exception("data index not found in memory.init");
                    }
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                // [i32 t i32] -> []
                case Opcode::Table_fill : {
                    const Instr::Table_fill& ins = std::get<Instr::Table_fill>(instr);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.fill");
                    }
                    const TableType& type = context.tables[ins.index].get();
                    state.pop(std::vector<ValueType> {ValueType::i32, (ValueType)type.reftype, ValueType::i32});
                }break;
                // [t t i32] -> [t]
                case Opcode::Select : {
                    const Instr::Select& ins = std::get<Instr::Select>(instr);
                    state.pop(ValueType::i32);
                    if(ins.valtypes.empty()){
                        State::StackValue op1 = state.pop<State::StackValue>();
                        State::StackValue op2 = state.pop<State::StackValue>();
                        if(!is_num(op1) || !is_num(op2)){
                            throw Exception::Exception("select without explicit types declared should be numeric types");
                        }
                        if(!is_unknown(op1) && !is_unknown(op2) && (std::get<ValueType>(op1) != std::get<ValueType>(op2))){
                            throw Exception::Exception("select types mismatch");
                        }
                        state.push(is_unknown(op1) ? op2 : op1);
                    }else{
                        std::vector<State::StackValue> ops1 = state.pop(ins.valtypes);
                        std::vector<State::StackValue> ops2 = state.pop(ins.valtypes);
                        for(size_t i = 0; i < ops1.size(); ++i){
                            State::StackValue& op1 = ops1[i];
                            State::StackValue& op2 = ops2[i];
                            if(!is_unknown(op1) && !is_unknown(op2) && (std::get<ValueType>(op1) != std::get<ValueType>(op2))){
                                throw Exception::Exception("select types mismatch");
                            }
                            state.push(is_unknown(op1) ? op2 : op1);
                        }
                    }
                }break;
                // [t1*] -> [t2*]
                case Opcode::Block : {
                    const Instr::Block& ins = std::get<Instr::Block>(instr);
                    FuncType type;
                    if(ins.type){
                        if(ins.type.value() >= context.types.size()){
                            throw Exception::Exception("type index not found for block type");
                        }
                        type = context.types[ins.type.value()];
                    }
                    state.pop(type.params);
                    state.push(Opcode::Block, type);
                    state.push(type.params);
                }break;
                case Opcode::Loop : {
                    const Instr::Loop& ins = std::get<Instr::Loop>(instr);
                    FuncType type;
                    if(ins.type){
                        if(ins.type.value() >= context.types.size()){
                            throw Exception::Exception("type index not found for loop type");
                        }
                        type = context.types[ins.type.value()];
                    }
                    state.pop(type.params);
                    state.push(Opcode::Loop, type);
                    state.push(type.params);
                }break;
                case Opcode::Else : {
                    State::CtrlFrame ctrl = state.pop<State::CtrlFrame>();
                    if(ctrl.opcode != Opcode::If){
                        throw Exception::Exception("else should be used with if");
                    }
                    state.push(Opcode::If, ctrl.type);
                    state.push(ctrl.type.params);
                }break;
                case Opcode::Call : {
                    const Instr::Call& ins = std::get<Instr::Call>(instr);
                    if(ins.index >= context.funcs.size()){
                        throw Exception::Exception("function index not found for call");
                    }
                    FuncType type = context.funcs[ins.index].get();
                    state.pop(type.params);
                    state.push(type.results);
                }break;
                case Opcode::Call_indirect : {
                    const Instr::Call_indirect& ins = std::get<Instr::Call_indirect>(instr);
                    if(ins.tableidx >= context.tables.size()){
                        throw Exception::Exception("table index not found for call_indirect");
                    }
                    if(context.tables[ins.tableidx].get().reftype != RefType::funcref){
                        throw Exception::Exception("table type should be funcref for call_indirect");
                    }
                    if(ins.typeidx >= context.types.size()){
                        throw Exception::Exception("type index not found for call_indirect");
                    }
                    FuncType type = context.types[ins.typeidx];
                    state.pop(ValueType::i32);
                    state.pop(type.params);
                    state.push(type.results);
                }break;
                // [t1* i32] -> [t2*]
                case Opcode::If : {
                    state.pop(ValueType::i32);
                    const Instr::If& ins = std::get<Instr::If>(instr);
                    FuncType type;
                    if(ins.type){
                        if(ins.type.value() >= context.types.size()){
                            throw Exception::Exception("type index not found for if type");
                        }
                        type = context.types[ins.type.value()];
                    }
                    state.pop(type.params);
                    state.push(Opcode::If, type);
                    state.push(type.params);
                }break;
                // unreachable
                case Opcode::Unreachable :
                    state.unreachable();
                break;
                case Opcode::Br : {
                    const Instr::Br& ins = std::get<Instr::Br>(instr);
                    if(ins.index >= state.ctrls.size()){
                        throw Exception::Exception("label index not found in br");
                    }
                    state.pop(state.ctrls[state.ctrls.size() - 1 - ins.index].types());
                    state.unreachable();
                }break;
                case Opcode::Br_table : {
                    state.pop(ValueType::i32);
                    const Instr::Br_table& ins = std::get<Instr::Br_table>(instr);
                    index_t m = ins.indices.back();
                    if(m >= state.ctrls.size()){
                        std::stringstream ss;
                        ss << "label index " << m << "not found in br_table";
                        throw Exception::Exception(ss.str());
                    }
                    size_t arity = state.ctrls[m].types().size();
                    for(auto it = ins.indices.begin(); it != (ins.indices.end() - 1); it = std::next(it)){
                        if(*it >= state.ctrls.size()){
                            std::stringstream ss;
                            ss << "label index " << *it << "not found in br_table";
                            throw Exception::Exception(ss.str());
                        }
                        std::vector<ValueType> label = state.ctrls[*it].types();
                        if(label.size() != arity){
                            std::stringstream ss;
                            ss << "label index " << *it << " has invalid type to br_table";
                            throw Exception::Exception(ss.str());
                        }
                        state.push(state.pop(label));
                    }
                    state.pop(state.ctrls[m].types());
                    state.unreachable();
                }break;
                case Opcode::Return :
                    state.pop(state.ctrls.front().type.results);
                    state.unreachable();
                break;
                default:
                // nop
                break;
            }
        }catch(Exception::Exception e){
            std::stringstream ss;
            ss << instridx << ": " << e.what();
            throw Exception::Exception(ss.str());
        }
        instridx += 1;
    }
}