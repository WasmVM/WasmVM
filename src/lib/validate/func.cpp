// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "exception.hpp"
#include "state.hpp"

#include <vector>
#include <cassert>

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
    for(const WasmInstr& instr : func.body){
        Opcode::opcode_t opcode = std::visit(overloaded {[](auto& ins){return ins.opcode;}}, instr);
        switch(opcode){
            // [] -> []
            case Opcode::Elem_drop : {
                const Instr::Elem_drop& ins = std::get<Instr::Elem_drop>(instr);
                if(ins.index >= context.elems.size()){
                    throw Exception::Validate_index_not_found("elem index not found in elem.drop");
                }
            }break;
            // [] -> [i32]
            case Opcode::I32_const :
                state.push(ValueType::i32);
            break;
            case Opcode::Table_size : {
                const Instr::Table_size& ins = std::get<Instr::Table_size>(instr);
                if(ins.index >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.size");
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
                    throw Exception::Validate_index_not_found("func index not found in ref.func");
                }
                state.push(ValueType::funcref);
            }break;
            // [] -> [t]
            case Opcode::Ref_null : {
                const Instr::Ref_null& ins = std::get<Instr::Ref_null>(instr);
                state.push((ValueType)ins.heaptype);
            }break;
            // [i32] -> [t]
            case Opcode::Table_get : {
                const Instr::Table_get& ins = std::get<Instr::Table_get>(instr);
                if(ins.index >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.get");
                }
                const TableType& type = context.tables[ins.index].get();
                state.pop(ValueType::i32);
                state.push((ValueType)type.reftype);
            }break;
            // [i32 t] -> []
            case Opcode::Table_set : {
                const Instr::Table_set& ins = std::get<Instr::Table_set>(instr);
                if(ins.index >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.set");
                }
                const TableType& type = context.tables[ins.index].get();
                state.pop(std::vector<ValueType> {ValueType::i32, (ValueType)type.reftype});
            }break;
            // [t] -> []
            case Opcode::Drop :
                state.pop<State::StackValue>();
            break;
            // [t*] -> []
            case Opcode::End : {
                State::CtrlFrame ctrl = state.pop<State::CtrlFrame>();
                state.push(ctrl.type.results);
            }break;
            // [t] -> [i32]
            case Opcode::Ref_is_null :
                if(!is_ref(state.pop<State::StackValue>())){
                    throw Exception::Validate_invalid_type("ref.is_null should consume reference type");
                }
                state.push(ValueType::i32);
            break;
            // [t i32] -> [i32]
            case Opcode::Table_grow : {
                const Instr::Table_grow& ins = std::get<Instr::Table_grow>(instr);
                if(ins.index >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.grow");
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
                    throw Exception::Validate_index_not_found("label index not found in br_if");
                }
                std::vector<ValueType> label_type = state.ctrls[state.ctrls.size() - 1 - ins.index].types();
                state.pop(label_type);
                state.push(label_type);
            }break;
            // [i32 i32 i32] -> []
            case Opcode::Table_copy : {
                const Instr::Table_copy& ins = std::get<Instr::Table_copy>(instr);
                if(ins.dstidx >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table destination index not found in table.copy");
                }
                if(ins.srcidx >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table source index not found in table.copy");
                }
                if(context.tables[ins.dstidx].get().reftype != context.tables[ins.srcidx].get().reftype){
                    throw Exception::Validate_type_not_match("table types not match in table.copy");
                };
                state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
            }break;
            case Opcode::Table_init : {
                const Instr::Table_init& ins = std::get<Instr::Table_init>(instr);
                if(ins.tableidx >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.init");
                }
                if(ins.elemidx >= context.elems.size()){
                    throw Exception::Validate_index_not_found("elem source index not found in table.init");
                }
                if(context.tables[ins.tableidx].get().reftype != context.elems[ins.elemidx].type){
                    throw Exception::Validate_type_not_match("table & elem types not match in table.init");
                };
                state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
            }break;
            // [i32 t i32] -> []
            case Opcode::Table_fill : {
                const Instr::Table_fill& ins = std::get<Instr::Table_fill>(instr);
                if(ins.index >= context.tables.size()){
                    throw Exception::Validate_index_not_found("table index not found in table.fill");
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
                        throw Exception::Validate_invalid_type("select without explicit types declared should be numeric types");
                    }
                    if(!is_unknown(op1) && !is_unknown(op2) && (std::get<ValueType>(op1) != std::get<ValueType>(op2))){
                        throw Exception::Validate_type_not_match("select types mismatch");
                    }
                    state.push(is_unknown(op1) ? op2 : op1);
                }else{
                    std::vector<State::StackValue> ops1 = state.pop(ins.valtypes);
                    std::vector<State::StackValue> ops2 = state.pop(ins.valtypes);
                    for(size_t i = 0; i < ops1.size(); ++i){
                        State::StackValue& op1 = ops1[i];
                        State::StackValue& op2 = ops2[i];
                        if(!is_unknown(op1) && !is_unknown(op2) && (std::get<ValueType>(op1) != std::get<ValueType>(op2))){
                            throw Exception::Validate_type_not_match("select types mismatch");
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
                        throw Exception::Validate_index_not_found("type index not found for block type");
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
                        throw Exception::Validate_index_not_found("type index not found for loop type");
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
                    throw Exception::Validate_invalid_operation("else should be used with if");
                }
                state.push(Opcode::If, ctrl.type);
                state.push(ctrl.type.params);
            }break;
            // [t1* i32] -> [t2*]
            case Opcode::If : {
                state.pop(ValueType::i32);
                const Instr::If& ins = std::get<Instr::If>(instr);
                FuncType type;
                if(ins.type){
                    if(ins.type.value() >= context.types.size()){
                        throw Exception::Validate_index_not_found("type index not found for if type");
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
                    throw Exception::Validate_index_not_found("label index not found in br");
                }
                state.pop(state.ctrls[state.ctrls.size() - 1 - ins.index].types());
                state.unreachable();
            }break;
            default:
            // nop
            break;
        }
    }
}