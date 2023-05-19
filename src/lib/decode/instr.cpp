// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

#include <Util.hpp>
#include <variant>

using namespace WasmVM;
using namespace Decode;

using BlockType = std::optional<index_t>;

#define AtomicInstr(I) \
    case Opcode::I : \
        instr.emplace<Instr::I>(); \
    break;

#define OneIndexInstr(I) \
    case Opcode::I : \
        instr.emplace<Instr::I>(stream.get<index_t>()); \
    break;

#define MemoryInstr(I) \
    case Opcode::I :{ \
        u32_t align = stream.get<u32_t>(); \
        if(align >= 0x40){ \
            instr.emplace<Instr::I>(stream.get<u32_t>(), stream.get<u32_t>(), align - 0x40); \
        }else{ \
            instr.emplace<Instr::I>(0, stream.get<u32_t>(), align); \
        } \
    }break;

template<> Opcode::opcode_t Stream::get<Opcode::opcode_t>(){
    Opcode::opcode_t opcode = (Opcode::opcode_t) get<byte_t>();
    if(opcode == 0xfc){
        opcode = (opcode << 8) | (Opcode::opcode_t) get<byte_t>();
    }
    return opcode;
}

template<> BlockType Stream::get<BlockType>(){
    u8_t prefix = (u8_t)peek();
    if(prefix == 0x40){
        istream.seekg(1, std::ios_base::cur);
        return std::nullopt;
    }else if(!(prefix & 0x80) && (prefix & 0x40)){
        ValueType valuetype = get<ValueType>();
        if(typemap.contains(valuetype)){
            return typemap[valuetype];
        }else{
            FuncType target;
            target.results.emplace_back(valuetype);
            index_t idx = 0;
            for(; idx < module.types.size(); ++idx){
                if(module.types[idx] == target){
                    typemap[valuetype] = idx;
                    return idx;
                }
            }
            typemap[valuetype] = idx;
            module.types.emplace_back(target);
            return idx;
        }
    }else{
        return (index_t)get<i64_t>();
    }
}

template<> Stream& Decode::operator>> <WasmInstr>(Stream& stream, WasmInstr& instr){
    switch(stream.get<Opcode::opcode_t>()){
        AtomicInstr(Unreachable)
        AtomicInstr(Nop)
        AtomicInstr(Else)
        AtomicInstr(End)
        AtomicInstr(Return)
        AtomicInstr(Ref_is_null)
        AtomicInstr(Drop)
        AtomicInstr(Select)
        AtomicInstr(I32_eqz)
        AtomicInstr(I32_eq)
        AtomicInstr(I32_ne)
        AtomicInstr(I32_lt_s)
        AtomicInstr(I32_lt_u)
        AtomicInstr(I32_gt_s)
        AtomicInstr(I32_gt_u)
        AtomicInstr(I32_le_s)
        AtomicInstr(I32_le_u)
        AtomicInstr(I32_ge_s)
        AtomicInstr(I32_ge_u)
        AtomicInstr(I64_eqz)
        AtomicInstr(I64_eq)
        AtomicInstr(I64_ne)
        AtomicInstr(I64_lt_s)
        AtomicInstr(I64_lt_u)
        AtomicInstr(I64_gt_s)
        AtomicInstr(I64_gt_u)
        AtomicInstr(I64_le_s)
        AtomicInstr(I64_le_u)
        AtomicInstr(I64_ge_s)
        AtomicInstr(I64_ge_u)
        AtomicInstr(F32_eq)
        AtomicInstr(F32_ne)
        AtomicInstr(F32_lt)
        AtomicInstr(F32_gt)
        AtomicInstr(F32_le)
        AtomicInstr(F32_ge)
        AtomicInstr(F64_eq)
        AtomicInstr(F64_ne)
        AtomicInstr(F64_lt)
        AtomicInstr(F64_gt)
        AtomicInstr(F64_le)
        AtomicInstr(F64_ge)
        AtomicInstr(I32_clz)
        AtomicInstr(I32_ctz)
        AtomicInstr(I32_popcnt)
        AtomicInstr(I32_add)
        AtomicInstr(I32_sub)
        AtomicInstr(I32_mul)
        AtomicInstr(I32_div_s)
        AtomicInstr(I32_div_u)
        AtomicInstr(I32_rem_s)
        AtomicInstr(I32_rem_u)
        AtomicInstr(I32_and)
        AtomicInstr(I32_or)
        AtomicInstr(I32_xor)
        AtomicInstr(I32_shl)
        AtomicInstr(I32_shr_s)
        AtomicInstr(I32_shr_u)
        AtomicInstr(I32_rotl)
        AtomicInstr(I32_rotr)
        AtomicInstr(I64_clz)
        AtomicInstr(I64_ctz)
        AtomicInstr(I64_popcnt)
        AtomicInstr(I64_add)
        AtomicInstr(I64_sub)
        AtomicInstr(I64_mul)
        AtomicInstr(I64_div_s)
        AtomicInstr(I64_div_u)
        AtomicInstr(I64_rem_s)
        AtomicInstr(I64_rem_u)
        AtomicInstr(I64_and)
        AtomicInstr(I64_or)
        AtomicInstr(I64_xor)
        AtomicInstr(I64_shl)
        AtomicInstr(I64_shr_s)
        AtomicInstr(I64_shr_u)
        AtomicInstr(I64_rotl)
        AtomicInstr(I64_rotr)
        AtomicInstr(F32_abs)
        AtomicInstr(F32_neg)
        AtomicInstr(F32_ceil)
        AtomicInstr(F32_floor)
        AtomicInstr(F32_trunc)
        AtomicInstr(F32_nearest)
        AtomicInstr(F32_sqrt)
        AtomicInstr(F32_add)
        AtomicInstr(F32_sub)
        AtomicInstr(F32_mul)
        AtomicInstr(F32_div)
        AtomicInstr(F32_min)
        AtomicInstr(F32_max)
        AtomicInstr(F32_copysign)
        AtomicInstr(F64_abs)
        AtomicInstr(F64_neg)
        AtomicInstr(F64_ceil)
        AtomicInstr(F64_floor)
        AtomicInstr(F64_trunc)
        AtomicInstr(F64_nearest)
        AtomicInstr(F64_sqrt)
        AtomicInstr(F64_add)
        AtomicInstr(F64_sub)
        AtomicInstr(F64_mul)
        AtomicInstr(F64_div)
        AtomicInstr(F64_min)
        AtomicInstr(F64_max)
        AtomicInstr(F64_copysign)
        AtomicInstr(I32_wrap_i64)
        AtomicInstr(I32_trunc_s_f32)
        AtomicInstr(I32_trunc_u_f32)
        AtomicInstr(I32_trunc_s_f64)
        AtomicInstr(I32_trunc_u_f64)
        AtomicInstr(I64_extend_s_i32)
        AtomicInstr(I64_extend_u_i32)
        AtomicInstr(I64_trunc_s_f32)
        AtomicInstr(I64_trunc_u_f32)
        AtomicInstr(I64_trunc_s_f64)
        AtomicInstr(I64_trunc_u_f64)
        AtomicInstr(F32_convert_s_i32)
        AtomicInstr(F32_convert_u_i32)
        AtomicInstr(F32_convert_s_i64)
        AtomicInstr(F32_convert_u_i64)
        AtomicInstr(F32_demote_f64)
        AtomicInstr(F64_convert_s_i32)
        AtomicInstr(F64_convert_u_i32)
        AtomicInstr(F64_convert_s_i64)
        AtomicInstr(F64_convert_u_i64)
        AtomicInstr(F64_promote_f32)
        AtomicInstr(I32_reinterpret_f32)
        AtomicInstr(I64_reinterpret_f64)
        AtomicInstr(F32_reinterpret_i32)
        AtomicInstr(F64_reinterpret_i64)
        AtomicInstr(I32_extend8_s)
        AtomicInstr(I32_extend16_s)
        AtomicInstr(I64_extend8_s)
        AtomicInstr(I64_extend16_s)
        AtomicInstr(I64_extend32_s)
        AtomicInstr(I32_trunc_sat_f32_s)
        AtomicInstr(I32_trunc_sat_f32_u)
        AtomicInstr(I32_trunc_sat_f64_s)
        AtomicInstr(I32_trunc_sat_f64_u)
        AtomicInstr(I64_trunc_sat_f32_s)
        AtomicInstr(I64_trunc_sat_f32_u)
        AtomicInstr(I64_trunc_sat_f64_s)
        AtomicInstr(I64_trunc_sat_f64_u)
        OneIndexInstr(Call)
        OneIndexInstr(Br)
        OneIndexInstr(Br_if)
        OneIndexInstr(Ref_func)
        OneIndexInstr(Local_get)
        OneIndexInstr(Local_set)
        OneIndexInstr(Local_tee)
        OneIndexInstr(Global_get)
        OneIndexInstr(Global_set)
        OneIndexInstr(Table_get)
        OneIndexInstr(Table_set)
        OneIndexInstr(Table_size)
        OneIndexInstr(Table_grow)
        OneIndexInstr(Table_fill)
        OneIndexInstr(Elem_drop)
        OneIndexInstr(Memory_size)
        OneIndexInstr(Memory_grow)
        OneIndexInstr(Memory_fill)
        OneIndexInstr(Data_drop)
        MemoryInstr(I32_load)
        MemoryInstr(I64_load)
        MemoryInstr(F32_load)
        MemoryInstr(F64_load)
        MemoryInstr(I32_load8_s)
        MemoryInstr(I32_load8_u)
        MemoryInstr(I32_load16_s)
        MemoryInstr(I32_load16_u)
        MemoryInstr(I64_load8_s)
        MemoryInstr(I64_load8_u)
        MemoryInstr(I64_load16_s)
        MemoryInstr(I64_load16_u)
        MemoryInstr(I64_load32_s)
        MemoryInstr(I64_load32_u)
        MemoryInstr(I32_store)
        MemoryInstr(I64_store)
        MemoryInstr(F32_store)
        MemoryInstr(F64_store)
        MemoryInstr(I32_store8)
        MemoryInstr(I32_store16)
        MemoryInstr(I64_store8)
        MemoryInstr(I64_store16)
        MemoryInstr(I64_store32)
        case Opcode::Block :
            instr.emplace<Instr::Block>(stream.get<BlockType>());
        break;
        case Opcode::Loop :
            instr.emplace<Instr::Loop>(stream.get<BlockType>());
        break;
        case Opcode::If :
            instr.emplace<Instr::If>(stream.get<BlockType>());
        break;
        case Opcode::Br_table : {
            Instr::Br_table& br_table = instr.emplace<Instr::Br_table>();
            stream >> br_table.indices;
            stream >> br_table.indices.emplace_back();
        }break;
        case Opcode::Call_indirect : {
            index_t typeidx = stream.get<index_t>();
            instr.emplace<Instr::Call_indirect>(stream.get<index_t>(), typeidx);
        }break;
        case Opcode::Ref_null :
            instr.emplace<Instr::Ref_null>(stream.get<RefType>());
        break;
        case Opcode::Select_t : {
            Instr::Select& ins = instr.emplace<Instr::Select>();
            stream >> ins.valtypes;
        }break;
        case Opcode::Table_copy : {
            instr.emplace<Instr::Table_copy>(stream.get<index_t>(), stream.get<index_t>());
        }break;
        case Opcode::Table_init : {
            index_t elemidx = stream.get<index_t>();
            instr.emplace<Instr::Table_init>(stream.get<index_t>(), elemidx);
        }break;
        case Opcode::Memory_copy : {
            instr.emplace<Instr::Memory_copy>(stream.get<index_t>(), stream.get<index_t>());
        }break;
        case Opcode::Memory_init : {
            index_t dataidx = stream.get<index_t>();
            instr.emplace<Instr::Memory_init>(stream.get<index_t>(), dataidx);
        }break;
        case Opcode::I32_const :
            instr.emplace<Instr::I32_const>(stream.get<i32_t>());
        break;
        case Opcode::I64_const :
            instr.emplace<Instr::I64_const>(stream.get<i64_t>());
        break;
        case Opcode::F32_const :
            instr.emplace<Instr::F32_const>(stream.get<f32_t>());
        break;
        case Opcode::F64_const :
            instr.emplace<Instr::F64_const>(stream.get<f64_t>());
        break;
        default:
            throw Exception::unknown_instruction(stream.location());
    }
    return stream;
}

template<> Stream& Decode::operator>> <ConstInstr>(Stream& stream, ConstInstr& instr){
    switch(stream.get<Opcode::opcode_t>()){
        OneIndexInstr(Ref_func)
        OneIndexInstr(Global_get)
        case Opcode::Ref_null :
            instr.emplace<Instr::Ref_null>(stream.get<RefType>());
        break;
        case Opcode::I32_const :
            instr.emplace<Instr::I32_const>(stream.get<i32_t>());
        break;
        case Opcode::I64_const :
            instr.emplace<Instr::I64_const>(stream.get<i64_t>());
        break;
        case Opcode::F32_const :
            instr.emplace<Instr::F32_const>(stream.get<f32_t>());
        break;
        case Opcode::F64_const :
            instr.emplace<Instr::F64_const>(stream.get<f64_t>());
        break;
        default:
            throw Exception::unknown_instruction(stream.location());
    }
    if(stream.get<byte_t>() != (byte_t)Opcode::End){
        throw Exception::expression_not_end(stream.location());
    }
    return stream;
}

#undef AtomicInstr
#undef OneIndexInstr
#undef MemoryInstr

