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
    if(!std::holds_alternative<ValueType>(value)){
        return false;
    }
    ValueType& type = std::get<ValueType>(value);
    return (type == ValueType::i32) || (type == ValueType::i64) || (type == ValueType::f32) || (type == ValueType::f64);
}

static bool is_ref(Validate::State::StackValue value){
    if(is_unknown(value)){
        return true;
    }
    return std::get<ValueType>(value).is_ref();
}

// ── GC type helpers ─────────────────────────────────────────────────────────
static const StructType& struct_type_at(const Validate::Validator::Context& context, index_t index, const char* what){
    if(index >= context.types.size()){
        throw Exception::Exception(std::string("type index not found for ") + what);
    }
    if(!context.types[index].is_struct()){
        throw Exception::Exception(std::string("type index is not a struct type for ") + what);
    }
    return std::get<StructType>(context.types[index].comp);
}
static const ArrayType& array_type_at(const Validate::Validator::Context& context, index_t index, const char* what){
    if(index >= context.types.size()){
        throw Exception::Exception(std::string("type index not found for ") + what);
    }
    if(!context.types[index].is_array()){
        throw Exception::Exception(std::string("type index is not an array type for ") + what);
    }
    return std::get<ArrayType>(context.types[index].comp);
}
static const FieldType& struct_field_at(const Validate::Validator::Context& context, index_t typeidx, index_t fieldidx, const char* what){
    const StructType& type = struct_type_at(context, typeidx, what);
    if(fieldidx >= type.fields.size()){
        throw Exception::Exception(std::string("field index not found for ") + what);
    }
    return type.fields[fieldidx];
}
static bool is_packed(const StorageType& storage){
    return std::holds_alternative<PackedType>(storage);
}
// A packed field is read and written through i32.
static ValueType unpacked(const StorageType& storage){
    return is_packed(storage) ? ValueType::i32 : std::get<ValueType>(storage);
}
static bool is_ref_storage(const StorageType& storage){
    if(is_packed(storage)) return false;
    ValueType type = std::get<ValueType>(storage);
    return (type == ValueType::funcref) || (type == ValueType::externref)
        || (type == ValueType::exnref) || (type == ValueType::i31ref);
}
// get on a packed field must say _s or _u; get_s / get_u need a packed field.
static void check_packed_access(bool packed, bool signed_or_unsigned, const char* what){
    if(packed && !signed_or_unsigned){
        throw Exception::Exception(std::string(what) + " on a packed field: use the _s or _u form");
    }
    if(!packed && signed_or_unsigned){
        throw Exception::Exception(std::string(what) + " needs a packed field");
    }
}

// The function type a type index names, for instructions whose immediate can
// only be a function type (block types, call_indirect, call_ref).
static const FuncType& func_type_at(const Validate::Validator::Context& context, index_t index, const char* what){
    if(index >= context.types.size()){
        throw Exception::Exception(std::string("type index not found for ") + what);
    }
    if(!context.types[index].is_func()){
        throw Exception::Exception(std::string("type index is not a function type for ") + what);
    }
    return context.types[index].func();
}

template<> void Validate::Validator::operator()<WasmFunc>(const WasmFunc& func){
    // type
    assert(func.typeidx < context.types.size());
    FuncType func_type = context.types[func.typeidx].func();
    // state (context C')
    State state(context.types);
    state.locals.insert(state.locals.end(), func_type.params.begin(), func_type.params.end());
    state.locals.insert(state.locals.end(), func.locals.begin(), func.locals.end());
    state.push(Opcode::Nop, func_type);
    // body
    std::stringstream msg;
    size_t instridx = 1;
    for(const WasmInstr& instr : func.body){
        try{
            switch(instr.opcode){
                // [] -> []
                case Opcode::Elem_drop : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.elems.size()){
                        throw Exception::Exception("elem index not found in elem.drop");
                    }
                }break;
                case Opcode::Data_drop : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.datas.size()){
                        throw Exception::Exception("data index not found in data.drop");
                    }
                }break;
                // [] -> [i32]
                case Opcode::I32_const :
                    state.push(ValueType::i32);
                break;
                case Opcode::Memory_size : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.size");
                    }
                    state.push(context.mems[ins.index].is64 ? ValueType::i64 : ValueType::i32);
                }break;
                case Opcode::Table_size : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
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
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.funcs.size()){
                        throw Exception::Exception("func index not found in ref.func");
                    }
                    state.push(ValueType::funcref);
                }break;
                // [] -> [t]
                case Opcode::Ref_null : {
                    const auto& ins = std::get<WasmInstr::HeapI32>(instr.imm);
                    if(!abstract_heaptype(ins.heaptype) && ((index_t)ins.heaptype >= context.types.size())){
                        throw Exception::Exception("type index not found for ref.null");
                    }
                    state.push(reference_to(ins.heaptype, true));
                }break;
                case Opcode::Local_get : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.get");
                    }
                    state.push(state.locals[ins.index]);
                }break;
                case Opcode::Global_get : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.globals.size()){
                        throw Exception::Exception("global index not found in global.get");
                    }
                    state.push(context.globals[ins.index].type);
                }break;
                // [i32|i64] -> [i32|i64]  (depends on memory index type)
                case Opcode::Memory_grow : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.grow");
                    }
                    ValueType addr_t = context.mems[ins.index].is64 ? ValueType::i64 : ValueType::i32;
                    state.pop(addr_t);
                    state.push(addr_t);
                }break;
                case Opcode::I32_clz :
                case Opcode::I32_ctz :
                case Opcode::I32_popcnt :
                case Opcode::I32_eqz :
                case Opcode::I32_extend8_s :
                case Opcode::I32_extend16_s :
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                break;
                // [i32] -> [i64]
                case Opcode::I64_extend_i32_s :
                case Opcode::I64_extend_i32_u :
                    state.pop(ValueType::i32);
                    state.push(ValueType::i64);
                break;
                // [i32] -> [f32]
                case Opcode::F32_convert_i32_s :
                case Opcode::F32_convert_i32_u :
                case Opcode::F32_reinterpret_i32 :
                    state.pop(ValueType::i32);
                    state.push(ValueType::f32);
                break;
                // [i32] -> [f64]
                case Opcode::F64_convert_i32_s :
                case Opcode::F64_convert_i32_u :
                    state.pop(ValueType::i32);
                    state.push(ValueType::f64);
                break;
                // [i64] -> [i64]
                case Opcode::I64_clz :
                case Opcode::I64_ctz :
                case Opcode::I64_popcnt :
                case Opcode::I64_extend8_s :
                case Opcode::I64_extend16_s :
                case Opcode::I64_extend32_s :
                    state.pop(ValueType::i64);
                    state.push(ValueType::i64);
                break;
                // [i64] -> [i32]
                case Opcode::I64_eqz :
                case Opcode::I32_wrap_i64 :
                    state.pop(ValueType::i64);
                    state.push(ValueType::i32);
                break;
                // [i64] -> [f32]
                case Opcode::F32_convert_i64_s :
                case Opcode::F32_convert_i64_u :
                    state.pop(ValueType::i64);
                    state.push(ValueType::f32);
                break;
                // [i64] -> [f64]
                case Opcode::F64_convert_i64_s :
                case Opcode::F64_convert_i64_u :
                case Opcode::F64_reinterpret_i64 :
                    state.pop(ValueType::i64);
                    state.push(ValueType::f64);
                break;
                // [f32] -> [f32]
                case Opcode::F32_abs :
                case Opcode::F32_neg :
                case Opcode::F32_ceil :
                case Opcode::F32_floor :
                case Opcode::F32_trunc :
                case Opcode::F32_nearest :
                case Opcode::F32_sqrt :
                    state.pop(ValueType::f32);
                    state.push(ValueType::f32);
                break;
                // [f32] -> [i32]
                case Opcode::I32_trunc_f32_s :
                case Opcode::I32_trunc_f32_u :
                case Opcode::I32_trunc_sat_f32_s :
                case Opcode::I32_trunc_sat_f32_u :
                case Opcode::I32_reinterpret_f32 :
                    state.pop(ValueType::f32);
                    state.push(ValueType::i32);
                break;
                // [f32] -> [i64]
                case Opcode::I64_trunc_f32_s :
                case Opcode::I64_trunc_f32_u :
                case Opcode::I64_trunc_sat_f32_s :
                case Opcode::I64_trunc_sat_f32_u :
                    state.pop(ValueType::f32);
                    state.push(ValueType::i64);
                break;
                // [f32] -> [f64]
                case Opcode::F64_promote_f32 :
                    state.pop(ValueType::f32);
                    state.push(ValueType::f64);
                break;
                // [f64] -> [f64]
                case Opcode::F64_abs :
                case Opcode::F64_neg :
                case Opcode::F64_ceil :
                case Opcode::F64_floor :
                case Opcode::F64_trunc :
                case Opcode::F64_nearest :
                case Opcode::F64_sqrt :
                    state.pop(ValueType::f64);
                    state.push(ValueType::f64);
                break;
                // [f64] -> [i32]
                case Opcode::I32_trunc_f64_s :
                case Opcode::I32_trunc_f64_u :
                case Opcode::I32_trunc_sat_f64_s :
                case Opcode::I32_trunc_sat_f64_u :
                    state.pop(ValueType::f64);
                    state.push(ValueType::i32);
                break;
                // [f64] -> [i64]
                case Opcode::I64_trunc_f64_s :
                case Opcode::I64_trunc_f64_u :
                case Opcode::I64_trunc_sat_f64_s :
                case Opcode::I64_trunc_sat_f64_u :
                case Opcode::I64_reinterpret_f64 :
                    state.pop(ValueType::f64);
                    state.push(ValueType::i64);
                break;
                // [f64] -> [f32]
                case Opcode::F32_demote_f64 :
                    state.pop(ValueType::f64);
                    state.push(ValueType::f32);
                break;
                // [i32 i32] -> [i32]
                case Opcode::I32_add :
                case Opcode::I32_sub :
                case Opcode::I32_mul :
                case Opcode::I32_div_s :
                case Opcode::I32_div_u :
                case Opcode::I32_rem_s :
                case Opcode::I32_rem_u :
                case Opcode::I32_and :
                case Opcode::I32_or :
                case Opcode::I32_xor :
                case Opcode::I32_shl :
                case Opcode::I32_shr_s :
                case Opcode::I32_shr_u :
                case Opcode::I32_rotl :
                case Opcode::I32_rotr :
                case Opcode::I32_eq :
                case Opcode::I32_ne :
                case Opcode::I32_lt_s :
                case Opcode::I32_lt_u :
                case Opcode::I32_gt_s :
                case Opcode::I32_gt_u :
                case Opcode::I32_le_s :
                case Opcode::I32_le_u :
                case Opcode::I32_ge_s :
                case Opcode::I32_ge_u :
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.push(ValueType::i32);
                break;
                // [i64 i64] -> [i64]
                case Opcode::I64_add :
                case Opcode::I64_sub :
                case Opcode::I64_mul :
                case Opcode::I64_div_s :
                case Opcode::I64_div_u :
                case Opcode::I64_rem_s :
                case Opcode::I64_rem_u :
                case Opcode::I64_and :
                case Opcode::I64_or :
                case Opcode::I64_xor :
                case Opcode::I64_shl :
                case Opcode::I64_shr_s :
                case Opcode::I64_shr_u :
                case Opcode::I64_rotl :
                case Opcode::I64_rotr :
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64);
                    state.push(ValueType::i64);
                break;
                // [i64 i64] -> [i32]
                case Opcode::I64_eq :
                case Opcode::I64_ne :
                case Opcode::I64_lt_s :
                case Opcode::I64_lt_u :
                case Opcode::I64_gt_s :
                case Opcode::I64_gt_u :
                case Opcode::I64_le_s :
                case Opcode::I64_le_u :
                case Opcode::I64_ge_s :
                case Opcode::I64_ge_u :
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64);
                    state.push(ValueType::i32);
                break;
                // [f32 f32] -> [f32]
                case Opcode::F32_add :
                case Opcode::F32_sub :
                case Opcode::F32_mul :
                case Opcode::F32_div :
                case Opcode::F32_min :
                case Opcode::F32_max :
                case Opcode::F32_copysign :
                    state.pop(ValueType::f32);
                    state.pop(ValueType::f32);
                    state.push(ValueType::f32);
                break;
                // [f32 f32] -> [i32]
                case Opcode::F32_eq :
                case Opcode::F32_ne :
                case Opcode::F32_lt :
                case Opcode::F32_gt :
                case Opcode::F32_le :
                case Opcode::F32_ge :
                    state.pop(ValueType::f32);
                    state.pop(ValueType::f32);
                    state.push(ValueType::i32);
                break;
                // [f64 f64] -> [f64]
                case Opcode::F64_add :
                case Opcode::F64_sub :
                case Opcode::F64_mul :
                case Opcode::F64_div :
                case Opcode::F64_min :
                case Opcode::F64_max :
                case Opcode::F64_copysign :
                    state.pop(ValueType::f64);
                    state.pop(ValueType::f64);
                    state.push(ValueType::f64);
                break;
                // [f64 f64] -> [i32]
                case Opcode::F64_eq :
                case Opcode::F64_ne :
                case Opcode::F64_lt :
                case Opcode::F64_gt :
                case Opcode::F64_le :
                case Opcode::F64_ge :
                    state.pop(ValueType::f64);
                    state.pop(ValueType::f64);
                    state.push(ValueType::i32);
                break;
                // [i32] -> [t]
                case Opcode::Table_get : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.get");
                    }
                    const TableType& type = context.tables[ins.index];
                    state.pop(ValueType::i32);
                    state.push(valtype_of(type.reftype));
                }break;
                // [i32|i64] -> [t]
                case Opcode::I32_load : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load8_s : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load8_u : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load16_s : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I32_load16_u : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i32);
                }break;
                case Opcode::I64_load : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-3");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load8_s : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load8_u : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load16_s : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load16_u : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load32_s : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::I64_load32_u : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::i64);
                }break;
                case Opcode::F32_load : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::f32);
                }break;
                case Opcode::F64_load : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-3");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.push(ValueType::f64);
                }break;
                // [i32 t] -> []
                case Opcode::Table_set : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.set");
                    }
                    const TableType& type = context.tables[ins.index];
                    state.pop(std::vector<ValueType> {ValueType::i32, valtype_of(type.reftype)});
                }break;
                // [i32|i64 t] -> []
                case Opcode::I32_store : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I32_store8 : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I32_store16 : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I64_store : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-3");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I64_store8 : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 0){
                        throw Exception::Exception("align should be 0");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I64_store16 : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 1){
                        throw Exception::Exception("align should be 0-1");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::I64_store32 : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::i64);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::F32_store : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 2){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::f32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::F64_store : {
                    const auto& ins = std::get<WasmInstr::MemArg>(instr.imm);
                    if(ins.memidx >= context.mems.size()){
                        throw Exception::Exception("memory index not found");
                    }
                    if(ins.align > 3){
                        throw Exception::Exception("align should be 0-2");
                    }
                    state.pop(ValueType::f64);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                // [t] -> []
                case Opcode::Drop :
                    state.pop<State::StackValue>();
                break;
                case Opcode::Local_set : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.set");
                    }
                    state.pop(state.locals[ins.index]);
                }break;
                case Opcode::Global_set : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.globals.size()){
                        throw Exception::Exception("global index not found in global.set");
                    }
                    GlobalType& global = context.globals[ins.index];
                    if(global.mut != GlobalType::variable){
                        throw Exception::Exception("global should be mutable in global.set");
                    }
                    state.pop(context.globals[ins.index].type);
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
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= state.locals.size()){
                        throw Exception::Exception("local index not found in local.tee");
                    }
                    state.push(state.pop(state.locals[ins.index]));
                }break;
                // [t i32] -> [i32]
                case Opcode::Table_grow : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.grow");
                    }
                    const TableType& type = context.tables[ins.index];
                    state.pop(std::vector<ValueType> {valtype_of(type.reftype), ValueType::i32});
                    state.push(ValueType::i32);
                }break;
                // [t* i32] -> [t*]
                case Opcode::Br_if : {
                    state.pop(ValueType::i32);
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= state.ctrls.size()){
                        throw Exception::Exception("label index not found in br_if");
                    }
                    std::vector<ValueType> label_type = state.ctrls[state.ctrls.size() - 1 - ins.index].types();
                    state.pop(label_type);
                    state.push(label_type);
                }break;
                // [i32 i32 i32] -> []
                case Opcode::Table_copy : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if(ins.a >= context.tables.size()){
                        throw Exception::Exception("table destination index not found in table.copy");
                    }
                    if(ins.b >= context.tables.size()){
                        throw Exception::Exception("table source index not found in table.copy");
                    }
                    if(context.tables[ins.a].reftype != context.tables[ins.b].reftype){
                        throw Exception::Exception("table types not match in table.copy");
                    };
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                case Opcode::Table_init : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if(ins.a >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.init");
                    }
                    if(ins.b >= context.elems.size()){
                        throw Exception::Exception("elem index not found in table.init");
                    }
                    if(context.tables[ins.a].reftype != context.elems[ins.b].type){
                        throw Exception::Exception("table & elem types not match in table.init");
                    };
                    state.pop(std::vector<ValueType> {ValueType::i32, ValueType::i32, ValueType::i32});
                }break;
                // [i32|i64 i32 i32|i64] -> []
                case Opcode::Memory_fill : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.fill");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                // [i32|i64 i32|i64 i32|i64] -> []
                case Opcode::Memory_copy : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if(ins.a >= context.mems.size()){
                        throw Exception::Exception("memory destination index not found in memory.copy");
                    }
                    if(ins.b >= context.mems.size()){
                        throw Exception::Exception("memory source index not found in memory.copy");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                case Opcode::Memory_init : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if(ins.a >= context.mems.size()){
                        throw Exception::Exception("memory index not found in memory.init");
                    }
                    if(ins.b >= context.datas.size()){
                        throw Exception::Exception("data index not found in memory.init");
                    }
                    state.pop(ValueType::i64, ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                    state.pop(ValueType::i64, ValueType::i32);
                }break;
                // [i32 t i32] -> []
                case Opcode::Table_fill : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.tables.size()){
                        throw Exception::Exception("table index not found in table.fill");
                    }
                    const TableType& type = context.tables[ins.index];
                    state.pop(std::vector<ValueType> {ValueType::i32, valtype_of(type.reftype), ValueType::i32});
                }break;
                // [t t i32] -> [t]
                case Opcode::Select : {
                    const auto& ins = std::get<WasmInstr::SelectV>(instr.imm);
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
                    const auto& ins = std::get<WasmInstr::BlockType>(instr.imm);
                    FuncType type;
                    if(ins.type){
                        type = func_type_at(context, ins.type.value(), "block type");
                    }
                    state.pop(type.params);
                    state.push(Opcode::Block, type);
                    state.push(type.params);
                }break;
                case Opcode::Loop : {
                    const auto& ins = std::get<WasmInstr::BlockType>(instr.imm);
                    FuncType type;
                    if(ins.type){
                        type = func_type_at(context, ins.type.value(), "loop type");
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
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.funcs.size()){
                        throw Exception::Exception("function index not found for call");
                    }
                    FuncType type = context.funcs[ins.index];
                    state.pop(type.params);
                    state.push(type.results);
                }break;
                case Opcode::Call_indirect : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if(ins.a >= context.tables.size()){
                        throw Exception::Exception("table index not found for call_indirect");
                    }
                    if(context.tables[ins.a].reftype != RefType::funcref){
                        throw Exception::Exception("table type should be funcref for call_indirect");
                    }
                    FuncType type = func_type_at(context, ins.b, "call_indirect");
                    state.pop(ValueType::i32);
                    state.pop(type.params);
                    state.push(type.results);
                }break;
                // call_ref: pop funcref + type's params, push type's results.
                case Opcode::Call_ref : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    FuncType type = func_type_at(context, ins.index, "call_ref");
                    state.pop(ValueType::funcref);
                    state.pop(type.params);
                    state.push(type.results);
                }break;
                // [t1* i32] -> [t2*]
                case Opcode::If : {
                    state.pop(ValueType::i32);
                    const auto& ins = std::get<WasmInstr::BlockType>(instr.imm);
                    FuncType type;
                    if(ins.type){
                        type = func_type_at(context, ins.type.value(), "if type");
                    }
                    state.pop(type.params);
                    state.push(Opcode::If, type);
                    state.push(type.params);
                }break;
                // [t1*] -> [t2*]
                case Opcode::Try_table : {
                    const auto& ins = std::get<WasmInstr::TryTab>(instr.imm);
                    FuncType type;
                    if(ins.type){
                        type = func_type_at(context, ins.type.value(), "try_table type");
                    }
                    // Per spec, catch label indices are resolved in the context
                    // enclosing the try_table (its own label is not referable),
                    // so validate them before pushing the try_table frame.
                    for(const Instr::TryCatchEntry& entry : ins.catches){
                        if(entry.label_idx >= state.ctrls.size()){
                            throw Exception::Exception("label index not found in try_table catch");
                        }
                        std::vector<ValueType> expect;
                        if((entry.kind == Instr::TryCatchEntry::Catch) || (entry.kind == Instr::TryCatchEntry::CatchRef)){
                            if(entry.tag_idx >= context.tags.size()){
                                throw Exception::Exception("tag index not found in try_table catch");
                            }
                            expect = context.tags[entry.tag_idx].params;
                        }
                        if((entry.kind == Instr::TryCatchEntry::CatchRef) || (entry.kind == Instr::TryCatchEntry::CatchAllRef)){
                            expect.emplace_back(ValueType::exnref);
                        }
                        if(state.ctrls[state.ctrls.size() - 1 - entry.label_idx].types() != expect){
                            throw Exception::Exception("catch label type not match in try_table");
                        }
                    }
                    state.pop(type.params);
                    state.push(Opcode::Try_table, type);
                    state.push(type.params);
                }break;
                // [t*] -> []
                case Opcode::Throw : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= context.tags.size()){
                        throw Exception::Exception("tag index not found in throw");
                    }
                    state.pop(context.tags[ins.index].params);
                    state.unreachable();
                }break;
                // [exnref] -> []
                case Opcode::Throw_ref :
                    state.pop(ValueType::exnref);
                    state.unreachable();
                break;
                // unreachable
                case Opcode::Unreachable :
                    state.unreachable();
                break;
                case Opcode::Br : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    if(ins.index >= state.ctrls.size()){
                        throw Exception::Exception("label index not found in br");
                    }
                    state.pop(state.ctrls[state.ctrls.size() - 1 - ins.index].types());
                    state.unreachable();
                }break;
                case Opcode::Br_table : {
                    state.pop(ValueType::i32);
                    const auto& ins = std::get<WasmInstr::BrTab>(instr.imm);
                    index_t m = ins.indices.back();
                    if(m >= state.ctrls.size()){
                        std::stringstream ss;
                        ss << "label index " << m << "not found in br_table";
                        throw Exception::Exception(ss.str());
                    }
                    size_t arity = state.ctrls[state.ctrls.size() - 1 - m].types().size();
                    for(auto it = ins.indices.begin(); it != (ins.indices.end() - 1); it = std::next(it)){
                        if(*it >= state.ctrls.size()){
                            std::stringstream ss;
                            ss << "label index " << *it << "not found in br_table";
                            throw Exception::Exception(ss.str());
                        }
                        std::vector<ValueType> label = state.ctrls[state.ctrls.size() - 1 - *it].types();
                        if(label.size() != arity){
                            std::stringstream ss;
                            ss << "label index " << *it << " has invalid type to br_table";
                            throw Exception::Exception(ss.str());
                        }
                        state.push(state.pop(label));
                    }
                    state.pop(state.ctrls[state.ctrls.size() - 1 - m].types());
                    state.unreachable();
                }break;
                case Opcode::Return :
                    state.pop(state.ctrls.front().type.results);
                    state.unreachable();
                break;
                // ── GC: struct ──────────────────────────────────────────────
                // [t*] -> [(ref $t)]
                case Opcode::Struct_new : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    const StructType& type = struct_type_at(context, ins.index, "struct.new");
                    for(auto it = type.fields.rbegin(); it != type.fields.rend(); ++it){
                        state.pop(unpacked(it->storage));
                    }
                    state.push(ValueType::concrete(ins.index, false));
                }break;
                // [] -> [(ref $t)]  (every storage type here has a default)
                case Opcode::Struct_new_default : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    struct_type_at(context, ins.index, "struct.new_default");
                    state.push(ValueType::concrete(ins.index, false));
                }break;
                // [(ref null $t)] -> [t]
                case Opcode::Struct_get : case Opcode::Struct_get_s : case Opcode::Struct_get_u : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = typeidx, b = fieldidx
                    const char* what = (instr.opcode == Opcode::Struct_get) ? "struct.get"
                                     : (instr.opcode == Opcode::Struct_get_s) ? "struct.get_s" : "struct.get_u";
                    const FieldType& field = struct_field_at(context, ins.a, ins.b, what);
                    check_packed_access(is_packed(field.storage), instr.opcode != Opcode::Struct_get, what);
                    state.pop(ValueType::concrete(ins.a, true));
                    state.push(unpacked(field.storage));
                }break;
                // [(ref null $t) t] -> []
                case Opcode::Struct_set : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm);
                    const FieldType& field = struct_field_at(context, ins.a, ins.b, "struct.set");
                    if(!field.mut){
                        throw Exception::Exception("struct.set on an immutable field");
                    }
                    state.pop(unpacked(field.storage));
                    state.pop(ValueType::concrete(ins.a, true));
                }break;
                // ── GC: array ───────────────────────────────────────────────
                // [t i32] -> [(ref $t)]
                case Opcode::Array_new : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    const ArrayType& type = array_type_at(context, ins.index, "array.new");
                    state.pop(ValueType::i32);
                    state.pop(unpacked(type.field.storage));
                    state.push(ValueType::concrete(ins.index, false));
                }break;
                // [i32] -> [(ref $t)]
                case Opcode::Array_new_default : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    array_type_at(context, ins.index, "array.new_default");
                    state.pop(ValueType::i32);
                    state.push(ValueType::concrete(ins.index, false));
                }break;
                // [t^n] -> [(ref $t)]
                case Opcode::Array_new_fixed : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = typeidx, b = n
                    const ArrayType& type = array_type_at(context, ins.a, "array.new_fixed");
                    for(u32_t n = ins.b; n > 0; --n){
                        state.pop(unpacked(type.field.storage));
                    }
                    state.push(ValueType::concrete(ins.a, false));
                }break;
                // [i32 i32] -> [(ref $t)]
                case Opcode::Array_new_data : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = typeidx, b = dataidx
                    const ArrayType& type = array_type_at(context, ins.a, "array.new_data");
                    if(is_ref_storage(type.field.storage)){
                        throw Exception::Exception("array.new_data needs a numeric or packed element type");
                    }
                    if(ins.b >= context.datas.size()){
                        throw Exception::Exception("data index not found for array.new_data");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.push(ValueType::concrete(ins.a, false));
                }break;
                case Opcode::Array_new_elem : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = typeidx, b = elemidx
                    const ArrayType& type = array_type_at(context, ins.a, "array.new_elem");
                    if(!is_ref_storage(type.field.storage)){
                        throw Exception::Exception("array.new_elem needs a reference element type");
                    }
                    if(ins.b >= context.elems.size()){
                        throw Exception::Exception("elem index not found for array.new_elem");
                    }
                    const ValueType& element = std::get<ValueType>(type.field.storage);
                        if(!is_legacy_reftype(element) || (reftype_of(element) != context.elems[ins.b].type)){
                        throw Exception::Exception("array.new_elem element type does not match the elem segment");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.push(ValueType::concrete(ins.a, false));
                }break;
                // [(ref null $t) i32] -> [t]
                case Opcode::Array_get : case Opcode::Array_get_s : case Opcode::Array_get_u : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    const char* what = (instr.opcode == Opcode::Array_get) ? "array.get"
                                     : (instr.opcode == Opcode::Array_get_s) ? "array.get_s" : "array.get_u";
                    const ArrayType& type = array_type_at(context, ins.index, what);
                    check_packed_access(is_packed(type.field.storage), instr.opcode != Opcode::Array_get, what);
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.index, true));
                    state.push(unpacked(type.field.storage));
                }break;
                // [(ref null $t) i32 t] -> []
                case Opcode::Array_set : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    const ArrayType& type = array_type_at(context, ins.index, "array.set");
                    if(!type.field.mut){
                        throw Exception::Exception("array.set on an immutable array");
                    }
                    state.pop(unpacked(type.field.storage));
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.index, true));
                }break;
                // [(ref null array)] -> [i32]
                case Opcode::Array_len : {
                    // Any array reference: the abstract arrayref, or a
                    // concrete reference to an array type.
                    State::StackValue ref = state.pop<State::StackValue>();
                    if(!is_unknown(ref)
                        && !type_matches(std::get<ValueType>(ref), ValueType(ValueType::arrayref), context.types)){
                        throw Exception::Exception("array.len needs an array reference");
                    }
                    state.push(ValueType::i32);
                }break;
                // [(ref null $t) i32 t i32] -> []
                case Opcode::Array_fill : {
                    const auto& ins = std::get<WasmInstr::OneIdx>(instr.imm);
                    const ArrayType& type = array_type_at(context, ins.index, "array.fill");
                    if(!type.field.mut){
                        throw Exception::Exception("array.fill on an immutable array");
                    }
                    state.pop(ValueType::i32);
                    state.pop(unpacked(type.field.storage));
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.index, true));
                }break;
                // [(ref null $d) i32 (ref null $s) i32 i32] -> []
                case Opcode::Array_copy : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = dst typeidx, b = src typeidx
                    const ArrayType& dst = array_type_at(context, ins.a, "array.copy");
                    const ArrayType& src = array_type_at(context, ins.b, "array.copy");
                    if(!dst.field.mut){
                        throw Exception::Exception("array.copy into an immutable array");
                    }
                    if(!(dst.field.storage == src.field.storage)){
                        throw Exception::Exception("array.copy between different element types");
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.b, true));
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.a, true));
                }break;
                // [(ref null $t) i32 i32 i32] -> []
                case Opcode::Array_init_data : case Opcode::Array_init_elem : {
                    const auto& ins = std::get<WasmInstr::TwoIdx>(instr.imm); // a = typeidx, b = data/elem idx
                    bool data = (instr.opcode == Opcode::Array_init_data);
                    const char* what = data ? "array.init_data" : "array.init_elem";
                    const ArrayType& type = array_type_at(context, ins.a, what);
                    if(!type.field.mut){
                        throw Exception::Exception(std::string(what) + " into an immutable array");
                    }
                    if(data){
                        if(is_ref_storage(type.field.storage)){
                            throw Exception::Exception("array.init_data needs a numeric or packed element type");
                        }
                        if(ins.b >= context.datas.size()){
                            throw Exception::Exception("data index not found for array.init_data");
                        }
                    }else{
                        if(!is_ref_storage(type.field.storage)){
                            throw Exception::Exception("array.init_elem needs a reference element type");
                        }
                        if(ins.b >= context.elems.size()){
                            throw Exception::Exception("elem index not found for array.init_elem");
                        }
                        const ValueType& element = std::get<ValueType>(type.field.storage);
                        if(!is_legacy_reftype(element) || (reftype_of(element) != context.elems[ins.b].type)){
                            throw Exception::Exception("array.init_elem element type does not match the elem segment");
                        }
                    }
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.pop(ValueType::i32);
                    state.pop(ValueType::concrete(ins.a, true));
                }break;
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