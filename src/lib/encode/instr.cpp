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
        }else if((instr.opcode >> 8) == 0xfb){ \
            return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff); \
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
AtomicInstr(WasmVM::Instr::Throw_ref)
AtomicInstr(WasmVM::Instr::Ref_is_null)
AtomicInstr(WasmVM::Instr::Ref_eq)
AtomicInstr(WasmVM::Instr::Ref_as_non_null)
AtomicInstr(WasmVM::Instr::Ref_i31)
AtomicInstr(WasmVM::Instr::I31_get_s)
AtomicInstr(WasmVM::Instr::I31_get_u)
AtomicInstr(WasmVM::Instr::Any_convert_extern)
AtomicInstr(WasmVM::Instr::Extern_convert_any)
AtomicInstr(WasmVM::Instr::Array_len)
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
AtomicInstr(WasmVM::Instr::I32_trunc_f32_s)
AtomicInstr(WasmVM::Instr::I32_trunc_f32_u)
AtomicInstr(WasmVM::Instr::I32_trunc_f64_s)
AtomicInstr(WasmVM::Instr::I32_trunc_f64_u)
AtomicInstr(WasmVM::Instr::I64_extend_i32_s)
AtomicInstr(WasmVM::Instr::I64_extend_i32_u)
AtomicInstr(WasmVM::Instr::I64_trunc_f32_s)
AtomicInstr(WasmVM::Instr::I64_trunc_f32_u)
AtomicInstr(WasmVM::Instr::I64_trunc_f64_s)
AtomicInstr(WasmVM::Instr::I64_trunc_f64_u)
AtomicInstr(WasmVM::Instr::F32_convert_i32_s)
AtomicInstr(WasmVM::Instr::F32_convert_i32_u)
AtomicInstr(WasmVM::Instr::F32_convert_i64_s)
AtomicInstr(WasmVM::Instr::F32_convert_i64_u)
AtomicInstr(WasmVM::Instr::F32_demote_f64)
AtomicInstr(WasmVM::Instr::F64_convert_i32_s)
AtomicInstr(WasmVM::Instr::F64_convert_i32_u)
AtomicInstr(WasmVM::Instr::F64_convert_i64_s)
AtomicInstr(WasmVM::Instr::F64_convert_i64_u)
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

OneIndexInstr(WasmVM::Instr::Throw)
OneIndexInstr(WasmVM::Instr::Struct_new)
OneIndexInstr(WasmVM::Instr::Struct_new_default)
OneIndexInstr(WasmVM::Instr::Array_new)
OneIndexInstr(WasmVM::Instr::Array_new_default)
OneIndexInstr(WasmVM::Instr::Array_get)
OneIndexInstr(WasmVM::Instr::Array_get_s)
OneIndexInstr(WasmVM::Instr::Array_get_u)
OneIndexInstr(WasmVM::Instr::Array_set)
OneIndexInstr(WasmVM::Instr::Call)
OneIndexInstr(WasmVM::Instr::Call_ref)
OneIndexInstr(WasmVM::Instr::Return_call)
OneIndexInstr(WasmVM::Instr::Return_call_ref)
OneIndexInstr(WasmVM::Instr::Br)
OneIndexInstr(WasmVM::Instr::Br_if)
OneIndexInstr(WasmVM::Instr::Br_on_null)
OneIndexInstr(WasmVM::Instr::Br_on_non_null)
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
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Try_table>(WasmVM::Instr::Try_table instr){
    *this << (byte_t)instr.opcode;
    if(instr.type){
        *this << (i64_t)instr.type.value();
    }else{
        *this << (byte_t) 0x40;
    }
    *this << (u32_t)instr.catches.size();
    for(const WasmVM::Instr::TryCatchEntry& ce : instr.catches){
        *this << (byte_t)ce.kind;
        if(ce.kind == 0 || ce.kind == 1){
            *this << ce.tag_idx;
        }
        *this << ce.label_idx;
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
    return *this << (byte_t)instr.opcode << instr.typeidx << instr.tableidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Return_call_indirect>(WasmVM::Instr::Return_call_indirect instr){
    return *this << (byte_t)instr.opcode << instr.typeidx << instr.tableidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_null>(WasmVM::Instr::Ref_null instr){
    return *this << (byte_t)instr.opcode << instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_test>(WasmVM::Instr::Ref_test instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (i64_t)instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_test_null>(WasmVM::Instr::Ref_test_null instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (i64_t)instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_cast>(WasmVM::Instr::Ref_cast instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (i64_t)instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Ref_cast_null>(WasmVM::Instr::Ref_cast_null instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (i64_t)instr.heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Br_on_cast>(WasmVM::Instr::Br_on_cast instr){
    u8_t flags = (instr.src_nullable ? 1 : 0) | (instr.dst_nullable ? 2 : 0);
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (byte_t)flags << instr.labelidx << (i64_t)instr.src_heaptype << (i64_t)instr.dst_heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Br_on_cast_fail>(WasmVM::Instr::Br_on_cast_fail instr){
    u8_t flags = (instr.src_nullable ? 1 : 0) | (instr.dst_nullable ? 2 : 0);
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << (byte_t)flags << instr.labelidx << (i64_t)instr.src_heaptype << (i64_t)instr.dst_heaptype;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Struct_get>(WasmVM::Instr::Struct_get instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.fieldidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Struct_get_s>(WasmVM::Instr::Struct_get_s instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.fieldidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Struct_get_u>(WasmVM::Instr::Struct_get_u instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.fieldidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Struct_set>(WasmVM::Instr::Struct_set instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.fieldidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_new_fixed>(WasmVM::Instr::Array_new_fixed instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.n;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_new_data>(WasmVM::Instr::Array_new_data instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.dataidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_new_elem>(WasmVM::Instr::Array_new_elem instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.elemidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_fill>(WasmVM::Instr::Array_fill instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_copy>(WasmVM::Instr::Array_copy instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.dst_typeidx << instr.src_typeidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_init_data>(WasmVM::Instr::Array_init_data instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.dataidx;
}
template<> Section::Stream& Section::Stream::operator<< <WasmVM::Instr::Array_init_elem>(WasmVM::Instr::Array_init_elem instr){
    return *this << (byte_t)0xfb << (u32_t)(instr.opcode & 0xff) << instr.typeidx << instr.elemidx;
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