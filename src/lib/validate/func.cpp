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
    FuncType type = context.types[func.typeidx];
    // state (context C')
    State state;
    state.locals.insert(state.locals.end(), type.params.begin(), type.params.end());
    state.locals.insert(state.locals.end(), func.locals.begin(), func.locals.end());
    FuncType type_r = type;
    type_r.params.clear();
    state.push(Opcode::Nop, type_r);
    // body
    for(const WasmInstr& instr : func.body){
        Opcode::opcode_t opcode = std::visit(overloaded {[](auto& ins){return ins.opcode;}}, instr);
        switch(opcode){
            // [] -> [i32]
            case Opcode::I32_const :
                state.push(ValueType::i32);
            break;
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
                        throw Exception::Validate_invalid_type("select types mismatch");
                    }
                    state.push(is_unknown(op1) ? op2 : op1);
                }else{
                    std::vector<State::StackValue> ops1 = state.pop(ins.valtypes);
                    std::vector<State::StackValue> ops2 = state.pop(ins.valtypes);
                    for(size_t i = 0; i < ops1.size(); ++i){
                        State::StackValue& op1 = ops1[i];
                        State::StackValue& op2 = ops2[i];
                        if(!is_unknown(op1) && !is_unknown(op2) && (std::get<ValueType>(op1) != std::get<ValueType>(op2))){
                            throw Exception::Validate_invalid_type("select types mismatch");
                        }
                        state.push(is_unknown(op1) ? op2 : op1);
                    }
                }
            }break;
            default:
            break;
        }
    }
}