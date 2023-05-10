// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"
#include "Util.hpp"

#include <variant>

using namespace WasmVM;
using namespace Encode;

#define AtomicInstr(T) \
    template<> Section::Stream& Section::Stream::operator<< <T>(T instr){ \
        if(instr.opcode <= 0xff){ \
            return *this << (byte_t)instr.opcode; \
        }else{ \
            return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff); \
        } \
    }

#define OneIndexInstr(T) \
    template<> Section::Stream& Section::Stream::operator<< <T>(T instr){ \
        if(instr.opcode <= 0xff){ \
            return *this << (byte_t)instr.opcode << instr.index; \
        }else{ \
            return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff) << instr.index; \
        } \
    }
#define MemoryInstr(T) \
    template<> Section::Stream& Section::Stream::operator<< <T>(T instr){ \
        if(instr.opcode <= 0xff){ \
            *this << (byte_t)instr.opcode; \
        }else{ \
            *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff); \
        } \
        if(instr.memidx == 0){ \
            *this << (u32_t)instr.align; \
        }else{ \
            *this << (u32_t)(instr.align | 0x40) << instr.memidx; \
        } \
        return *this << instr.offset; \
    }

AtomicInstr(WasmVM::Instr::Unreachable)
AtomicInstr(WasmVM::Instr::Nop)
AtomicInstr(WasmVM::Instr::Else)
AtomicInstr(WasmVM::Instr::End)
AtomicInstr(WasmVM::Instr::Return)
AtomicInstr(WasmVM::Instr::Ref_is_null)
AtomicInstr(WasmVM::Instr::Drop)
AtomicInstr(WasmVM::Instr::I32_eqz)
AtomicInstr(WasmVM::Instr::I32_eq)
AtomicInstr(WasmVM::Instr::I32_ne)
AtomicInstr(WasmVM::Instr::I32_lt_s)
AtomicInstr(WasmVM::Instr::I32_lt_u)
AtomicInstr(WasmVM::Instr::I32_gt_s)
AtomicInstr(WasmVM::Instr::I32_gt_u)
AtomicInstr(WasmVM::Instr::I32_le_s)
AtomicInstr(WasmVM::Instr::I32_le_u)
AtomicInstr(WasmVM::Instr::I32_ge_s)
AtomicInstr(WasmVM::Instr::I32_ge_u)
AtomicInstr(WasmVM::Instr::I64_eqz)
AtomicInstr(WasmVM::Instr::I64_eq)
AtomicInstr(WasmVM::Instr::I64_ne)
AtomicInstr(WasmVM::Instr::I64_lt_s)
AtomicInstr(WasmVM::Instr::I64_lt_u)
AtomicInstr(WasmVM::Instr::I64_gt_s)
AtomicInstr(WasmVM::Instr::I64_gt_u)
AtomicInstr(WasmVM::Instr::I64_le_s)
AtomicInstr(WasmVM::Instr::I64_le_u)
AtomicInstr(WasmVM::Instr::I64_ge_s)
AtomicInstr(WasmVM::Instr::I64_ge_u)
AtomicInstr(WasmVM::Instr::F32_eq)
AtomicInstr(WasmVM::Instr::F32_ne)
AtomicInstr(WasmVM::Instr::F32_lt)
AtomicInstr(WasmVM::Instr::F32_gt)
AtomicInstr(WasmVM::Instr::F32_le)
AtomicInstr(WasmVM::Instr::F32_ge)
AtomicInstr(WasmVM::Instr::F64_eq)
AtomicInstr(WasmVM::Instr::F64_ne)
AtomicInstr(WasmVM::Instr::F64_lt)
AtomicInstr(WasmVM::Instr::F64_gt)
AtomicInstr(WasmVM::Instr::F64_le)
AtomicInstr(WasmVM::Instr::F64_ge)
AtomicInstr(WasmVM::Instr::I32_clz)
AtomicInstr(WasmVM::Instr::I32_ctz)
AtomicInstr(WasmVM::Instr::I32_popcnt)
AtomicInstr(WasmVM::Instr::I32_add)
AtomicInstr(WasmVM::Instr::I32_sub)
AtomicInstr(WasmVM::Instr::I32_mul)
AtomicInstr(WasmVM::Instr::I32_div_s)
AtomicInstr(WasmVM::Instr::I32_div_u)
AtomicInstr(WasmVM::Instr::I32_rem_s)
AtomicInstr(WasmVM::Instr::I32_rem_u)
AtomicInstr(WasmVM::Instr::I32_and)
AtomicInstr(WasmVM::Instr::I32_or)
AtomicInstr(WasmVM::Instr::I32_xor)
AtomicInstr(WasmVM::Instr::I32_shl)
AtomicInstr(WasmVM::Instr::I32_shr_s)
AtomicInstr(WasmVM::Instr::I32_shr_u)
AtomicInstr(WasmVM::Instr::I32_rotl)
AtomicInstr(WasmVM::Instr::I32_rotr)
AtomicInstr(WasmVM::Instr::I64_clz)
AtomicInstr(WasmVM::Instr::I64_ctz)
AtomicInstr(WasmVM::Instr::I64_popcnt)
AtomicInstr(WasmVM::Instr::I64_add)
AtomicInstr(WasmVM::Instr::I64_sub)
AtomicInstr(WasmVM::Instr::I64_mul)
AtomicInstr(WasmVM::Instr::I64_div_s)
AtomicInstr(WasmVM::Instr::I64_div_u)
AtomicInstr(WasmVM::Instr::I64_rem_s)
AtomicInstr(WasmVM::Instr::I64_rem_u)
AtomicInstr(WasmVM::Instr::I64_and)
AtomicInstr(WasmVM::Instr::I64_or)
AtomicInstr(WasmVM::Instr::I64_xor)
AtomicInstr(WasmVM::Instr::I64_shl)
AtomicInstr(WasmVM::Instr::I64_shr_s)
AtomicInstr(WasmVM::Instr::I64_shr_u)
AtomicInstr(WasmVM::Instr::I64_rotl)
AtomicInstr(WasmVM::Instr::I64_rotr)
AtomicInstr(WasmVM::Instr::F32_abs)
AtomicInstr(WasmVM::Instr::F32_neg)
AtomicInstr(WasmVM::Instr::F32_ceil)
AtomicInstr(WasmVM::Instr::F32_floor)
AtomicInstr(WasmVM::Instr::F32_trunc)
AtomicInstr(WasmVM::Instr::F32_nearest)
AtomicInstr(WasmVM::Instr::F32_sqrt)
AtomicInstr(WasmVM::Instr::F32_add)
AtomicInstr(WasmVM::Instr::F32_sub)
AtomicInstr(WasmVM::Instr::F32_mul)
AtomicInstr(WasmVM::Instr::F32_div)
AtomicInstr(WasmVM::Instr::F32_min)
AtomicInstr(WasmVM::Instr::F32_max)
AtomicInstr(WasmVM::Instr::F32_copysign)
AtomicInstr(WasmVM::Instr::F64_abs)
AtomicInstr(WasmVM::Instr::F64_neg)
AtomicInstr(WasmVM::Instr::F64_ceil)
AtomicInstr(WasmVM::Instr::F64_floor)
AtomicInstr(WasmVM::Instr::F64_trunc)
AtomicInstr(WasmVM::Instr::F64_nearest)
AtomicInstr(WasmVM::Instr::F64_sqrt)
AtomicInstr(WasmVM::Instr::F64_add)
AtomicInstr(WasmVM::Instr::F64_sub)
AtomicInstr(WasmVM::Instr::F64_mul)
AtomicInstr(WasmVM::Instr::F64_div)
AtomicInstr(WasmVM::Instr::F64_min)
AtomicInstr(WasmVM::Instr::F64_max)
AtomicInstr(WasmVM::Instr::F64_copysign)
AtomicInstr(WasmVM::Instr::I32_wrap_i64)
AtomicInstr(WasmVM::Instr::I32_trunc_s_f32)
AtomicInstr(WasmVM::Instr::I32_trunc_u_f32)
AtomicInstr(WasmVM::Instr::I32_trunc_s_f64)
AtomicInstr(WasmVM::Instr::I32_trunc_u_f64)
AtomicInstr(WasmVM::Instr::I64_extend_s_i32)
AtomicInstr(WasmVM::Instr::I64_extend_u_i32)
AtomicInstr(WasmVM::Instr::I64_trunc_s_f32)
AtomicInstr(WasmVM::Instr::I64_trunc_u_f32)
AtomicInstr(WasmVM::Instr::I64_trunc_s_f64)
AtomicInstr(WasmVM::Instr::I64_trunc_u_f64)
AtomicInstr(WasmVM::Instr::F32_convert_s_i32)
AtomicInstr(WasmVM::Instr::F32_convert_u_i32)
AtomicInstr(WasmVM::Instr::F32_convert_s_i64)
AtomicInstr(WasmVM::Instr::F32_convert_u_i64)
AtomicInstr(WasmVM::Instr::F32_demote_f64)
AtomicInstr(WasmVM::Instr::F64_convert_s_i32)
AtomicInstr(WasmVM::Instr::F64_convert_u_i32)
AtomicInstr(WasmVM::Instr::F64_convert_s_i64)
AtomicInstr(WasmVM::Instr::F64_convert_u_i64)
AtomicInstr(WasmVM::Instr::F64_promote_f32)
AtomicInstr(WasmVM::Instr::I32_reinterpret_f32)
AtomicInstr(WasmVM::Instr::I64_reinterpret_f64)
AtomicInstr(WasmVM::Instr::F32_reinterpret_i32)
AtomicInstr(WasmVM::Instr::F64_reinterpret_i64)
AtomicInstr(WasmVM::Instr::I32_extend8_s)
AtomicInstr(WasmVM::Instr::I32_extend16_s)
AtomicInstr(WasmVM::Instr::I64_extend8_s)
AtomicInstr(WasmVM::Instr::I64_extend16_s)
AtomicInstr(WasmVM::Instr::I64_extend32_s)
AtomicInstr(WasmVM::Instr::I32_trunc_sat_f32_s)
AtomicInstr(WasmVM::Instr::I32_trunc_sat_f32_u)
AtomicInstr(WasmVM::Instr::I32_trunc_sat_f64_s)
AtomicInstr(WasmVM::Instr::I32_trunc_sat_f64_u)
AtomicInstr(WasmVM::Instr::I64_trunc_sat_f32_s)
AtomicInstr(WasmVM::Instr::I64_trunc_sat_f32_u)
AtomicInstr(WasmVM::Instr::I64_trunc_sat_f64_s)
AtomicInstr(WasmVM::Instr::I64_trunc_sat_f64_u)

OneIndexInstr(WasmVM::Instr::Call)
OneIndexInstr(WasmVM::Instr::Br)
OneIndexInstr(WasmVM::Instr::Br_if)
OneIndexInstr(WasmVM::Instr::Ref_func)
OneIndexInstr(WasmVM::Instr::Local_get)
OneIndexInstr(WasmVM::Instr::Local_set)
OneIndexInstr(WasmVM::Instr::Local_tee)
OneIndexInstr(WasmVM::Instr::Global_get)
OneIndexInstr(WasmVM::Instr::Global_set)
OneIndexInstr(WasmVM::Instr::Table_get)
OneIndexInstr(WasmVM::Instr::Table_set)
OneIndexInstr(WasmVM::Instr::Table_size)
OneIndexInstr(WasmVM::Instr::Table_grow)
OneIndexInstr(WasmVM::Instr::Table_fill)
OneIndexInstr(WasmVM::Instr::Elem_drop)
OneIndexInstr(WasmVM::Instr::Memory_size)
OneIndexInstr(WasmVM::Instr::Memory_grow)
OneIndexInstr(WasmVM::Instr::Memory_fill)
OneIndexInstr(WasmVM::Instr::Data_drop)

MemoryInstr(WasmVM::Instr::I32_load)
MemoryInstr(WasmVM::Instr::I64_load)
MemoryInstr(WasmVM::Instr::F32_load)
MemoryInstr(WasmVM::Instr::F64_load)
MemoryInstr(WasmVM::Instr::I32_load8_s)
MemoryInstr(WasmVM::Instr::I32_load8_u)
MemoryInstr(WasmVM::Instr::I32_load16_s)
MemoryInstr(WasmVM::Instr::I32_load16_u)
MemoryInstr(WasmVM::Instr::I64_load8_s)
MemoryInstr(WasmVM::Instr::I64_load8_u)
MemoryInstr(WasmVM::Instr::I64_load16_s)
MemoryInstr(WasmVM::Instr::I64_load16_u)
MemoryInstr(WasmVM::Instr::I64_load32_s)
MemoryInstr(WasmVM::Instr::I64_load32_u)
MemoryInstr(WasmVM::Instr::I32_store)
MemoryInstr(WasmVM::Instr::I64_store)
MemoryInstr(WasmVM::Instr::F32_store)
MemoryInstr(WasmVM::Instr::F64_store)
MemoryInstr(WasmVM::Instr::I32_store8)
MemoryInstr(WasmVM::Instr::I32_store16)
MemoryInstr(WasmVM::Instr::I64_store8)
MemoryInstr(WasmVM::Instr::I64_store16)
MemoryInstr(WasmVM::Instr::I64_store32)

#undef AtomicInstr
#undef OneIndexInstr
#undef MemoryInstr

template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Block>(WasmVM::Instr::Block instr){
    *this << (byte_t)instr.opcode;
    if(instr.type){
        *this << (i64_t)instr.type.value();
    }else{
        *this << (byte_t) 0x40;
    }
    return *this;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Loop>(WasmVM::Instr::Loop instr){
    *this << (byte_t)instr.opcode;
    if(instr.type){
        *this << (i64_t)instr.type.value();
    }else{
        *this << (byte_t) 0x40;
    }
    return *this;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::If>(WasmVM::Instr::If instr){
    *this << (byte_t)instr.opcode;
    if(instr.type){
        *this << (i64_t)instr.type.value();
    }else{
        *this << (byte_t) 0x40;
    }
    return *this;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Br_table>(WasmVM::Instr::Br_table instr){
    *this << (byte_t)instr.opcode << (u32_t)(instr.indices.size() - 1);
    for(index_t index : instr.indices){
        *this << index;
    }
    return *this;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Call_indirect>(WasmVM::Instr::Call_indirect instr){
    return *this << (byte_t)instr.opcode << instr.tableidx << instr.typeidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_null>(WasmVM::Instr::Ref_null instr){
    return *this << (byte_t)instr.opcode << instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Select>(WasmVM::Instr::Select instr){
    if(instr.valtypes.empty()){
        return *this << (byte_t)instr.opcode;
    }else{
        return *this << (byte_t)Opcode::Select_t << instr.valtypes;
    }
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Table_copy>(WasmVM::Instr::Table_copy instr){
    return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff) << instr.dstidx << instr.srcidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Table_init>(WasmVM::Instr::Table_init instr){
    return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff) << instr.elemidx << instr.tableidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Memory_copy>(WasmVM::Instr::Memory_copy instr){
    return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff) << instr.dstidx << instr.srcidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Memory_init>(WasmVM::Instr::Memory_init instr){
    return *this << (byte_t)0xfc << (byte_t)(instr.opcode & 0xff) << instr.dataidx << instr.memidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::I32_const>(WasmVM::Instr::I32_const instr){
    return *this << (byte_t)instr.opcode << instr.value;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::I64_const>(WasmVM::Instr::I64_const instr){
    return *this << (byte_t)instr.opcode << instr.value;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::F32_const>(WasmVM::Instr::F32_const instr){
    return *this << (byte_t)instr.opcode << instr.value;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::F64_const>(WasmVM::Instr::F64_const instr){
    return *this << (byte_t)instr.opcode << instr.value;
}

template<> Section::Stream& Section::Stream::operator<< <WasmInstr>(WasmInstr instr){
    std::visit(overloaded {
        [&](auto ins){
            *this << ins;
        }
    }, instr);
    return *this;
}

template<> Section::Stream& Section::Stream::operator<< <ConstInstr>(ConstInstr instr){
    std::visit(overloaded {
        [&](auto ins){
            *this << ins;
        }
    }, instr);
    return *this << (byte_t)Opcode::End;
}