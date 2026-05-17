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
// Reconstruct the appropriate Instr::* temporary and dispatch to its template specialization.
#define ATOM(OP, T) case Opcode::OP: { Instr::T t; return *this << t; }
#define ONE(OP, T) case Opcode::OP: { auto& _o = std::get<WasmInstr::OneIdx>(instr.imm); Instr::T t{_o.index}; return *this << t; }
#define TWO(OP, T, A, B) case Opcode::OP: { auto& _tw = std::get<WasmInstr::TwoIdx>(instr.imm); Instr::T t{_tw.A, _tw.B}; return *this << t; }
#define BLK(OP, T) case Opcode::OP: { auto& _b = std::get<WasmInstr::BlockType>(instr.imm); Instr::T t{_b.type}; return *this << t; }
#define MEM(OP, T) case Opcode::OP: { auto& _m = std::get<WasmInstr::MemArg>(instr.imm); Instr::T t{_m.memidx, _m.offset, _m.align}; return *this << t; }
    switch(instr.opcode) {
    ATOM(Unreachable,Unreachable) ATOM(Nop,Nop) ATOM(Else,Else) ATOM(End,End)
    ATOM(Return,Return) ATOM(Throw_ref,Throw_ref)
    ATOM(Ref_is_null,Ref_is_null) ATOM(Ref_eq,Ref_eq) ATOM(Ref_as_non_null,Ref_as_non_null)
    ATOM(Ref_i31,Ref_i31) ATOM(I31_get_s,I31_get_s) ATOM(I31_get_u,I31_get_u)
    ATOM(Any_convert_extern,Any_convert_extern) ATOM(Extern_convert_any,Extern_convert_any)
    ATOM(Array_len,Array_len) ATOM(Drop,Drop)
    BLK(Block,Block) BLK(Loop,Loop) BLK(If,If)
    ONE(Br,Br) ONE(Br_if,Br_if) ONE(Throw,Throw) ONE(Call,Call)
    ONE(Return_call,Return_call) ONE(Return_call_ref,Return_call_ref) ONE(Call_ref,Call_ref)
    ONE(Ref_func,Ref_func) ONE(Br_on_null,Br_on_null) ONE(Br_on_non_null,Br_on_non_null)
    ONE(Local_get,Local_get) ONE(Local_set,Local_set) ONE(Local_tee,Local_tee)
    ONE(Global_get,Global_get) ONE(Global_set,Global_set)
    ONE(Table_get,Table_get) ONE(Table_set,Table_set)
    ONE(Table_size,Table_size) ONE(Table_grow,Table_grow) ONE(Table_fill,Table_fill)
    ONE(Elem_drop,Elem_drop)
    ONE(Memory_size,Memory_size) ONE(Memory_grow,Memory_grow)
    ONE(Memory_fill,Memory_fill) ONE(Data_drop,Data_drop)
    ONE(Struct_new,Struct_new) ONE(Struct_new_default,Struct_new_default)
    ONE(Array_new,Array_new) ONE(Array_new_default,Array_new_default)
    ONE(Array_get,Array_get) ONE(Array_get_s,Array_get_s)
    ONE(Array_get_u,Array_get_u) ONE(Array_set,Array_set)
    ONE(Array_fill,Array_fill)
    TWO(Call_indirect,Call_indirect,a,b) TWO(Return_call_indirect,Return_call_indirect,a,b)
    TWO(Table_copy,Table_copy,a,b) TWO(Memory_copy,Memory_copy,a,b)
    TWO(Struct_get,Struct_get,a,b) TWO(Struct_get_s,Struct_get_s,a,b)
    TWO(Struct_get_u,Struct_get_u,a,b) TWO(Struct_set,Struct_set,a,b)
    TWO(Array_new_fixed,Array_new_fixed,a,b) TWO(Array_new_data,Array_new_data,a,b)
    TWO(Array_new_elem,Array_new_elem,a,b) TWO(Array_copy,Array_copy,a,b)
    TWO(Array_init_data,Array_init_data,a,b) TWO(Array_init_elem,Array_init_elem,a,b)
    TWO(Memory_init,Memory_init,b,a) // Memory_init encodes dataidx then memidx
    case Opcode::Table_init: {
        auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
        Instr::Table_init t{tw.a, tw.b};
        return *this << t;
    }
    MEM(I32_load,I32_load) MEM(I64_load,I64_load) MEM(F32_load,F32_load) MEM(F64_load,F64_load)
    MEM(I32_load8_s,I32_load8_s) MEM(I32_load8_u,I32_load8_u)
    MEM(I32_load16_s,I32_load16_s) MEM(I32_load16_u,I32_load16_u)
    MEM(I64_load8_s,I64_load8_s) MEM(I64_load8_u,I64_load8_u)
    MEM(I64_load16_s,I64_load16_s) MEM(I64_load16_u,I64_load16_u)
    MEM(I64_load32_s,I64_load32_s) MEM(I64_load32_u,I64_load32_u)
    MEM(I32_store,I32_store) MEM(I64_store,I64_store)
    MEM(F32_store,F32_store) MEM(F64_store,F64_store)
    MEM(I32_store8,I32_store8) MEM(I32_store16,I32_store16)
    MEM(I64_store8,I64_store8) MEM(I64_store16,I64_store16) MEM(I64_store32,I64_store32)
    case Opcode::I32_const: { auto& c=std::get<WasmInstr::ConstI32>(instr.imm); Instr::I32_const t{c.value}; return *this<<t; }
    case Opcode::I64_const: { auto& c=std::get<WasmInstr::ConstI64>(instr.imm); Instr::I64_const t{c.value}; return *this<<t; }
    case Opcode::F32_const: { auto& c=std::get<WasmInstr::ConstF32>(instr.imm); Instr::F32_const t{c.value}; return *this<<t; }
    case Opcode::F64_const: { auto& c=std::get<WasmInstr::ConstF64>(instr.imm); Instr::F64_const t{c.value}; return *this<<t; }
    case Opcode::Ref_null: { auto& h=std::get<WasmInstr::HeapRef>(instr.imm); Instr::Ref_null t{h.heaptype}; return *this<<t; }
    case Opcode::Ref_test: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_test t{h.heaptype}; return *this<<t; }
    case Opcode::Ref_test_null: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_test_null t{h.heaptype}; return *this<<t; }
    case Opcode::Ref_cast: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_cast t{h.heaptype}; return *this<<t; }
    case Opcode::Ref_cast_null: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_cast_null t{h.heaptype}; return *this<<t; }
    case Opcode::Br_on_cast: { auto& c=std::get<WasmInstr::CastOp>(instr.imm); Instr::Br_on_cast t{c.label,c.src_n,c.dst_n,c.src_ht,c.dst_ht}; return *this<<t; }
    case Opcode::Br_on_cast_fail: { auto& c=std::get<WasmInstr::CastOp>(instr.imm); Instr::Br_on_cast_fail t{c.label,c.src_n,c.dst_n,c.src_ht,c.dst_ht}; return *this<<t; }
    case Opcode::Br_table: { auto& bt=std::get<WasmInstr::BrTab>(instr.imm); Instr::Br_table t; t.indices=bt.indices; return *this<<t; }
    case Opcode::Try_table: { auto& tt=std::get<WasmInstr::TryTab>(instr.imm); Instr::Try_table t; t.type=tt.type; t.catches=tt.catches; return *this<<t; }
    case Opcode::Select: { Instr::Select t; return *this<<t; }
    ATOM(I32_eqz,I32_eqz) ATOM(I32_eq,I32_eq) ATOM(I32_ne,I32_ne)
    ATOM(I32_lt_s,I32_lt_s) ATOM(I32_lt_u,I32_lt_u) ATOM(I32_gt_s,I32_gt_s) ATOM(I32_gt_u,I32_gt_u)
    ATOM(I32_le_s,I32_le_s) ATOM(I32_le_u,I32_le_u) ATOM(I32_ge_s,I32_ge_s) ATOM(I32_ge_u,I32_ge_u)
    ATOM(I64_eqz,I64_eqz) ATOM(I64_eq,I64_eq) ATOM(I64_ne,I64_ne)
    ATOM(I64_lt_s,I64_lt_s) ATOM(I64_lt_u,I64_lt_u) ATOM(I64_gt_s,I64_gt_s) ATOM(I64_gt_u,I64_gt_u)
    ATOM(I64_le_s,I64_le_s) ATOM(I64_le_u,I64_le_u) ATOM(I64_ge_s,I64_ge_s) ATOM(I64_ge_u,I64_ge_u)
    ATOM(F32_eq,F32_eq) ATOM(F32_ne,F32_ne) ATOM(F32_lt,F32_lt) ATOM(F32_gt,F32_gt) ATOM(F32_le,F32_le) ATOM(F32_ge,F32_ge)
    ATOM(F64_eq,F64_eq) ATOM(F64_ne,F64_ne) ATOM(F64_lt,F64_lt) ATOM(F64_gt,F64_gt) ATOM(F64_le,F64_le) ATOM(F64_ge,F64_ge)
    ATOM(I32_clz,I32_clz) ATOM(I32_ctz,I32_ctz) ATOM(I32_popcnt,I32_popcnt)
    ATOM(I32_add,I32_add) ATOM(I32_sub,I32_sub) ATOM(I32_mul,I32_mul)
    ATOM(I32_div_s,I32_div_s) ATOM(I32_div_u,I32_div_u) ATOM(I32_rem_s,I32_rem_s) ATOM(I32_rem_u,I32_rem_u)
    ATOM(I32_and,I32_and) ATOM(I32_or,I32_or) ATOM(I32_xor,I32_xor)
    ATOM(I32_shl,I32_shl) ATOM(I32_shr_s,I32_shr_s) ATOM(I32_shr_u,I32_shr_u) ATOM(I32_rotl,I32_rotl) ATOM(I32_rotr,I32_rotr)
    ATOM(I64_clz,I64_clz) ATOM(I64_ctz,I64_ctz) ATOM(I64_popcnt,I64_popcnt)
    ATOM(I64_add,I64_add) ATOM(I64_sub,I64_sub) ATOM(I64_mul,I64_mul)
    ATOM(I64_div_s,I64_div_s) ATOM(I64_div_u,I64_div_u) ATOM(I64_rem_s,I64_rem_s) ATOM(I64_rem_u,I64_rem_u)
    ATOM(I64_and,I64_and) ATOM(I64_or,I64_or) ATOM(I64_xor,I64_xor)
    ATOM(I64_shl,I64_shl) ATOM(I64_shr_s,I64_shr_s) ATOM(I64_shr_u,I64_shr_u) ATOM(I64_rotl,I64_rotl) ATOM(I64_rotr,I64_rotr)
    ATOM(F32_abs,F32_abs) ATOM(F32_neg,F32_neg) ATOM(F32_ceil,F32_ceil) ATOM(F32_floor,F32_floor)
    ATOM(F32_trunc,F32_trunc) ATOM(F32_nearest,F32_nearest) ATOM(F32_sqrt,F32_sqrt)
    ATOM(F32_add,F32_add) ATOM(F32_sub,F32_sub) ATOM(F32_mul,F32_mul) ATOM(F32_div,F32_div)
    ATOM(F32_min,F32_min) ATOM(F32_max,F32_max) ATOM(F32_copysign,F32_copysign)
    ATOM(F64_abs,F64_abs) ATOM(F64_neg,F64_neg) ATOM(F64_ceil,F64_ceil) ATOM(F64_floor,F64_floor)
    ATOM(F64_trunc,F64_trunc) ATOM(F64_nearest,F64_nearest) ATOM(F64_sqrt,F64_sqrt)
    ATOM(F64_add,F64_add) ATOM(F64_sub,F64_sub) ATOM(F64_mul,F64_mul) ATOM(F64_div,F64_div)
    ATOM(F64_min,F64_min) ATOM(F64_max,F64_max) ATOM(F64_copysign,F64_copysign)
    ATOM(I32_wrap_i64,I32_wrap_i64)
    ATOM(I32_trunc_f32_s,I32_trunc_f32_s) ATOM(I32_trunc_f32_u,I32_trunc_f32_u)
    ATOM(I32_trunc_f64_s,I32_trunc_f64_s) ATOM(I32_trunc_f64_u,I32_trunc_f64_u)
    ATOM(I64_extend_i32_s,I64_extend_i32_s) ATOM(I64_extend_i32_u,I64_extend_i32_u)
    ATOM(I64_trunc_f32_s,I64_trunc_f32_s) ATOM(I64_trunc_f32_u,I64_trunc_f32_u)
    ATOM(I64_trunc_f64_s,I64_trunc_f64_s) ATOM(I64_trunc_f64_u,I64_trunc_f64_u)
    ATOM(F32_convert_i32_s,F32_convert_i32_s) ATOM(F32_convert_i32_u,F32_convert_i32_u)
    ATOM(F32_convert_i64_s,F32_convert_i64_s) ATOM(F32_convert_i64_u,F32_convert_i64_u)
    ATOM(F32_demote_f64,F32_demote_f64)
    ATOM(F64_convert_i32_s,F64_convert_i32_s) ATOM(F64_convert_i32_u,F64_convert_i32_u)
    ATOM(F64_convert_i64_s,F64_convert_i64_s) ATOM(F64_convert_i64_u,F64_convert_i64_u)
    ATOM(F64_promote_f32,F64_promote_f32)
    ATOM(I32_reinterpret_f32,I32_reinterpret_f32) ATOM(I64_reinterpret_f64,I64_reinterpret_f64)
    ATOM(F32_reinterpret_i32,F32_reinterpret_i32) ATOM(F64_reinterpret_i64,F64_reinterpret_i64)
    ATOM(I32_extend8_s,I32_extend8_s) ATOM(I32_extend16_s,I32_extend16_s)
    ATOM(I64_extend8_s,I64_extend8_s) ATOM(I64_extend16_s,I64_extend16_s) ATOM(I64_extend32_s,I64_extend32_s)
    ATOM(I32_trunc_sat_f32_s,I32_trunc_sat_f32_s) ATOM(I32_trunc_sat_f32_u,I32_trunc_sat_f32_u)
    ATOM(I32_trunc_sat_f64_s,I32_trunc_sat_f64_s) ATOM(I32_trunc_sat_f64_u,I32_trunc_sat_f64_u)
    ATOM(I64_trunc_sat_f32_s,I64_trunc_sat_f32_s) ATOM(I64_trunc_sat_f32_u,I64_trunc_sat_f32_u)
    ATOM(I64_trunc_sat_f64_s,I64_trunc_sat_f64_s) ATOM(I64_trunc_sat_f64_u,I64_trunc_sat_f64_u)
    default: break;
    }
#undef ATOM
#undef ONE
#undef TWO
#undef BLK
#undef MEM
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