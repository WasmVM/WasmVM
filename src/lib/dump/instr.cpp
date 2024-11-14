// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>
#include <cmath>

using namespace WasmVM;

struct InstrVisitor {
    InstrVisitor(std::ostream& stream) : stream(stream){}

#define AtomicInstr(T, S) \
    std::ostream& operator()(const T&){ \
        return stream << S; \
    }
#define OneIndexInstr(T, S) \
    std::ostream& operator()(const T& instr){ \
        return stream << S << " " << instr.index; \
    }
#define MemoryInstr(T, S) \
    std::ostream& operator()(const T& instr){ \
        return stream << S << " " << instr.memidx << " offset=" << (u64_t)instr.offset << " align=" << (u32_t)instr.align; \
    }

    AtomicInstr(WasmVM::Instr::Unreachable, "unreachable")
    AtomicInstr(WasmVM::Instr::Nop, "nop")
    AtomicInstr(WasmVM::Instr::Else, "else")
    AtomicInstr(WasmVM::Instr::End, "end")
    AtomicInstr(WasmVM::Instr::Return, "return")
    AtomicInstr(WasmVM::Instr::Ref_is_null, "ref.is_null")
    AtomicInstr(WasmVM::Instr::Drop, "drop")
    AtomicInstr(WasmVM::Instr::I32_eqz, "i32.eqz")
    AtomicInstr(WasmVM::Instr::I32_eq, "i32.eq")
    AtomicInstr(WasmVM::Instr::I32_ne, "i32.ne")
    AtomicInstr(WasmVM::Instr::I32_lt_s, "i32.lt_s")
    AtomicInstr(WasmVM::Instr::I32_lt_u, "i32.lt_u")
    AtomicInstr(WasmVM::Instr::I32_gt_s, "i32.gt_s")
    AtomicInstr(WasmVM::Instr::I32_gt_u, "i32.gt_u")
    AtomicInstr(WasmVM::Instr::I32_le_s, "i32.le_s")
    AtomicInstr(WasmVM::Instr::I32_le_u, "i32.le_u")
    AtomicInstr(WasmVM::Instr::I32_ge_s, "i32.ge_s")
    AtomicInstr(WasmVM::Instr::I32_ge_u, "i32.ge_u")
    AtomicInstr(WasmVM::Instr::I64_eqz, "i64.eqz")
    AtomicInstr(WasmVM::Instr::I64_eq, "i64.eq")
    AtomicInstr(WasmVM::Instr::I64_ne, "i64.ne")
    AtomicInstr(WasmVM::Instr::I64_lt_s, "i64.lt_s")
    AtomicInstr(WasmVM::Instr::I64_lt_u, "i64.lt_u")
    AtomicInstr(WasmVM::Instr::I64_gt_s, "i64.gt_s")
    AtomicInstr(WasmVM::Instr::I64_gt_u, "i64.gt_u")
    AtomicInstr(WasmVM::Instr::I64_le_s, "i64.le_s")
    AtomicInstr(WasmVM::Instr::I64_le_u, "i64.le_u")
    AtomicInstr(WasmVM::Instr::I64_ge_s, "i64.ge_s")
    AtomicInstr(WasmVM::Instr::I64_ge_u, "i64.ge_u")
    AtomicInstr(WasmVM::Instr::F32_eq, "f32.eq")
    AtomicInstr(WasmVM::Instr::F32_ne, "f32.ne")
    AtomicInstr(WasmVM::Instr::F32_lt, "f32.lt")
    AtomicInstr(WasmVM::Instr::F32_gt, "f32.gt")
    AtomicInstr(WasmVM::Instr::F32_le, "f32.le")
    AtomicInstr(WasmVM::Instr::F32_ge, "f32.ge")
    AtomicInstr(WasmVM::Instr::F64_eq, "f64.eq")
    AtomicInstr(WasmVM::Instr::F64_ne, "f64.ne")
    AtomicInstr(WasmVM::Instr::F64_lt, "f64.lt")
    AtomicInstr(WasmVM::Instr::F64_gt, "f64.gt")
    AtomicInstr(WasmVM::Instr::F64_le, "f64.le")
    AtomicInstr(WasmVM::Instr::F64_ge, "f64.ge")
    AtomicInstr(WasmVM::Instr::I32_clz, "i32.clz")
    AtomicInstr(WasmVM::Instr::I32_ctz, "i32.ctz")
    AtomicInstr(WasmVM::Instr::I32_popcnt, "i32.popcnt")
    AtomicInstr(WasmVM::Instr::I32_add, "i32.add")
    AtomicInstr(WasmVM::Instr::I32_sub, "i32.sub")
    AtomicInstr(WasmVM::Instr::I32_mul, "i32.mul")
    AtomicInstr(WasmVM::Instr::I32_div_s, "i32.div_s")
    AtomicInstr(WasmVM::Instr::I32_div_u, "i32.div_u")
    AtomicInstr(WasmVM::Instr::I32_rem_s, "i32.rem_s")
    AtomicInstr(WasmVM::Instr::I32_rem_u, "i32.rem_u")
    AtomicInstr(WasmVM::Instr::I32_and, "i32.and")
    AtomicInstr(WasmVM::Instr::I32_or, "i32.or")
    AtomicInstr(WasmVM::Instr::I32_xor, "i32.xor")
    AtomicInstr(WasmVM::Instr::I32_shl, "i32.shl")
    AtomicInstr(WasmVM::Instr::I32_shr_s, "i32.shr_s")
    AtomicInstr(WasmVM::Instr::I32_shr_u, "i32.shr_u")
    AtomicInstr(WasmVM::Instr::I32_rotl, "i32.rotl")
    AtomicInstr(WasmVM::Instr::I32_rotr, "i32.rotr")
    AtomicInstr(WasmVM::Instr::I64_clz, "i64.clz")
    AtomicInstr(WasmVM::Instr::I64_ctz, "i64.ctz")
    AtomicInstr(WasmVM::Instr::I64_popcnt, "i64.popcnt")
    AtomicInstr(WasmVM::Instr::I64_add, "i64.add")
    AtomicInstr(WasmVM::Instr::I64_sub, "i64.sub")
    AtomicInstr(WasmVM::Instr::I64_mul, "i64.mul")
    AtomicInstr(WasmVM::Instr::I64_div_s, "i64.div_s")
    AtomicInstr(WasmVM::Instr::I64_div_u, "i64.div_u")
    AtomicInstr(WasmVM::Instr::I64_rem_s, "i64.rem_s")
    AtomicInstr(WasmVM::Instr::I64_rem_u, "i64.rem_u")
    AtomicInstr(WasmVM::Instr::I64_and, "i64.and")
    AtomicInstr(WasmVM::Instr::I64_or, "i64.or")
    AtomicInstr(WasmVM::Instr::I64_xor, "i64.xor")
    AtomicInstr(WasmVM::Instr::I64_shl, "i64.shl")
    AtomicInstr(WasmVM::Instr::I64_shr_s, "i64.shr_s")
    AtomicInstr(WasmVM::Instr::I64_shr_u, "i64.shr_u")
    AtomicInstr(WasmVM::Instr::I64_rotl, "i64.rotl")
    AtomicInstr(WasmVM::Instr::I64_rotr, "i64.rotr")
    AtomicInstr(WasmVM::Instr::F32_abs, "f32.abs")
    AtomicInstr(WasmVM::Instr::F32_neg, "f32.neg")
    AtomicInstr(WasmVM::Instr::F32_ceil, "f32.ceil")
    AtomicInstr(WasmVM::Instr::F32_floor, "f32.floor")
    AtomicInstr(WasmVM::Instr::F32_trunc, "f32.trunc")
    AtomicInstr(WasmVM::Instr::F32_nearest, "f32.nearest")
    AtomicInstr(WasmVM::Instr::F32_sqrt, "f32.sqrt")
    AtomicInstr(WasmVM::Instr::F32_add, "f32.add")
    AtomicInstr(WasmVM::Instr::F32_sub, "f32.sub")
    AtomicInstr(WasmVM::Instr::F32_mul, "f32.mul")
    AtomicInstr(WasmVM::Instr::F32_div, "f32.div")
    AtomicInstr(WasmVM::Instr::F32_min, "f32.min")
    AtomicInstr(WasmVM::Instr::F32_max, "f32.max")
    AtomicInstr(WasmVM::Instr::F32_copysign, "f32.copysign")
    AtomicInstr(WasmVM::Instr::F64_abs, "f64.abs")
    AtomicInstr(WasmVM::Instr::F64_neg, "f64.neg")
    AtomicInstr(WasmVM::Instr::F64_ceil, "f64.ceil")
    AtomicInstr(WasmVM::Instr::F64_floor, "f64.floor")
    AtomicInstr(WasmVM::Instr::F64_trunc, "f64.trunc")
    AtomicInstr(WasmVM::Instr::F64_nearest, "f64.nearest")
    AtomicInstr(WasmVM::Instr::F64_sqrt, "f64.sqrt")
    AtomicInstr(WasmVM::Instr::F64_add, "f64.add")
    AtomicInstr(WasmVM::Instr::F64_sub, "f64.sub")
    AtomicInstr(WasmVM::Instr::F64_mul, "f64.mul")
    AtomicInstr(WasmVM::Instr::F64_div, "f64.div")
    AtomicInstr(WasmVM::Instr::F64_min, "f64.min")
    AtomicInstr(WasmVM::Instr::F64_max, "f64.max")
    AtomicInstr(WasmVM::Instr::F64_copysign, "f64.copysign")
    AtomicInstr(WasmVM::Instr::I32_wrap_i64, "i32.wrap_i64")
    AtomicInstr(WasmVM::Instr::I32_trunc_s_f32, "i32.trunc_s_f32")
    AtomicInstr(WasmVM::Instr::I32_trunc_u_f32, "i32.trunc_u_f32")
    AtomicInstr(WasmVM::Instr::I32_trunc_s_f64, "i32.trunc_s_f64")
    AtomicInstr(WasmVM::Instr::I32_trunc_u_f64, "i32.trunc_u_f64")
    AtomicInstr(WasmVM::Instr::I64_extend_s_i32, "i64.extend_s_i32")
    AtomicInstr(WasmVM::Instr::I64_extend_u_i32, "i64.extend_u_i32")
    AtomicInstr(WasmVM::Instr::I64_trunc_s_f32, "i64.trunc_s_f32")
    AtomicInstr(WasmVM::Instr::I64_trunc_u_f32, "i64.trunc_u_f32")
    AtomicInstr(WasmVM::Instr::I64_trunc_s_f64, "i64.trunc_s_f64")
    AtomicInstr(WasmVM::Instr::I64_trunc_u_f64, "i64.trunc_u_f64")
    AtomicInstr(WasmVM::Instr::F32_convert_s_i32, "f32.convert_s_i32")
    AtomicInstr(WasmVM::Instr::F32_convert_u_i32, "f32.convert_u_i32")
    AtomicInstr(WasmVM::Instr::F32_convert_s_i64, "f32.convert_s_i64")
    AtomicInstr(WasmVM::Instr::F32_convert_u_i64, "f32.convert_u_i64")
    AtomicInstr(WasmVM::Instr::F32_demote_f64, "f32.demote_f64")
    AtomicInstr(WasmVM::Instr::F64_convert_s_i32, "f64.convert_s_i32")
    AtomicInstr(WasmVM::Instr::F64_convert_u_i32, "f64.convert_u_i32")
    AtomicInstr(WasmVM::Instr::F64_convert_s_i64, "f64.convert_s_i64")
    AtomicInstr(WasmVM::Instr::F64_convert_u_i64, "f64.convert_u_i64")
    AtomicInstr(WasmVM::Instr::F64_promote_f32, "f64.promote_f32")
    AtomicInstr(WasmVM::Instr::I32_reinterpret_f32, "i32.reinterpret_f32")
    AtomicInstr(WasmVM::Instr::I64_reinterpret_f64, "i64.reinterpret_f64")
    AtomicInstr(WasmVM::Instr::F32_reinterpret_i32, "f32.reinterpret_i32")
    AtomicInstr(WasmVM::Instr::F64_reinterpret_i64, "f64.reinterpret_i64")
    AtomicInstr(WasmVM::Instr::I32_extend8_s, "i32.extend8_s")
    AtomicInstr(WasmVM::Instr::I32_extend16_s, "i32.extend16_s")
    AtomicInstr(WasmVM::Instr::I64_extend8_s, "i64.extend8_s")
    AtomicInstr(WasmVM::Instr::I64_extend16_s, "i64.extend16_s")
    AtomicInstr(WasmVM::Instr::I64_extend32_s, "i64.extend32_s")
    AtomicInstr(WasmVM::Instr::I32_trunc_sat_f32_s, "i32.trunc_sat_f32_s")
    AtomicInstr(WasmVM::Instr::I32_trunc_sat_f32_u, "i32.trunc_sat_f32_u")
    AtomicInstr(WasmVM::Instr::I32_trunc_sat_f64_s, "i32.trunc_sat_f64_s")
    AtomicInstr(WasmVM::Instr::I32_trunc_sat_f64_u, "i32.trunc_sat_f64_u")
    AtomicInstr(WasmVM::Instr::I64_trunc_sat_f32_s, "i64.trunc_sat_f32_s")
    AtomicInstr(WasmVM::Instr::I64_trunc_sat_f32_u, "i64.trunc_sat_f32_u")
    AtomicInstr(WasmVM::Instr::I64_trunc_sat_f64_s, "i64.trunc_sat_f64_s")
    AtomicInstr(WasmVM::Instr::I64_trunc_sat_f64_u, "i64.trunc_sat_f64_u")

    OneIndexInstr(WasmVM::Instr::Call, "call")
    OneIndexInstr(WasmVM::Instr::Br, "br")
    OneIndexInstr(WasmVM::Instr::Br_if, "br_if")
    OneIndexInstr(WasmVM::Instr::Ref_func, "ref.func")
    OneIndexInstr(WasmVM::Instr::Local_get, "local.get")
    OneIndexInstr(WasmVM::Instr::Local_set, "local.set")
    OneIndexInstr(WasmVM::Instr::Local_tee, "local.tee")
    OneIndexInstr(WasmVM::Instr::Global_get, "global.get")
    OneIndexInstr(WasmVM::Instr::Global_set, "global.set")
    OneIndexInstr(WasmVM::Instr::Table_get, "table.get")
    OneIndexInstr(WasmVM::Instr::Table_set, "table.set")
    OneIndexInstr(WasmVM::Instr::Table_size, "table.size")
    OneIndexInstr(WasmVM::Instr::Table_grow, "table.grow")
    OneIndexInstr(WasmVM::Instr::Table_fill, "table.fill")
    OneIndexInstr(WasmVM::Instr::Elem_drop, "elem.drop")
    OneIndexInstr(WasmVM::Instr::Memory_size, "memory.size")
    OneIndexInstr(WasmVM::Instr::Memory_grow, "memory.grow")
    OneIndexInstr(WasmVM::Instr::Memory_fill, "memory.fill")
    OneIndexInstr(WasmVM::Instr::Data_drop, "data.drop")

    MemoryInstr(WasmVM::Instr::I32_load, "i32.load")
    MemoryInstr(WasmVM::Instr::I64_load, "i64.load")
    MemoryInstr(WasmVM::Instr::F32_load, "f32.load")
    MemoryInstr(WasmVM::Instr::F64_load, "f64.load")
    MemoryInstr(WasmVM::Instr::I32_load8_s, "i32.load8_s")
    MemoryInstr(WasmVM::Instr::I32_load8_u, "i32.load8_u")
    MemoryInstr(WasmVM::Instr::I32_load16_s, "i32.load16_s")
    MemoryInstr(WasmVM::Instr::I32_load16_u, "i32.load16_u")
    MemoryInstr(WasmVM::Instr::I64_load8_s, "i64.load8_s")
    MemoryInstr(WasmVM::Instr::I64_load8_u, "i64.load8_u")
    MemoryInstr(WasmVM::Instr::I64_load16_s, "i64.load16_s")
    MemoryInstr(WasmVM::Instr::I64_load16_u, "i64.load16_u")
    MemoryInstr(WasmVM::Instr::I64_load32_s, "i64.load32_s")
    MemoryInstr(WasmVM::Instr::I64_load32_u, "i64.load32_u")
    MemoryInstr(WasmVM::Instr::I32_store, "i32.store")
    MemoryInstr(WasmVM::Instr::I64_store, "i64.store")
    MemoryInstr(WasmVM::Instr::F32_store, "f32.store")
    MemoryInstr(WasmVM::Instr::F64_store, "f64.store")
    MemoryInstr(WasmVM::Instr::I32_store8, "i32.store8")
    MemoryInstr(WasmVM::Instr::I32_store16, "i32.store16")
    MemoryInstr(WasmVM::Instr::I64_store8, "i64.store8")
    MemoryInstr(WasmVM::Instr::I64_store16, "i64.store16")
    MemoryInstr(WasmVM::Instr::I64_store32, "i64.store32")

    std::ostream& operator()(const WasmVM::Instr::Block& instr){
        stream << "block";
        if(instr.type){
            stream << " (type " << instr.type.value() << ")";
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::Loop& instr){
        stream << "loop";
        if(instr.type){
            stream << " (type " << instr.type.value() << ")";
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::If& instr){
        stream << "if";
        if(instr.type){
            stream << " (type " << instr.type.value() << ")";
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::Br_table& instr){
        stream << "br_table";
        for(index_t index : instr.indices){
            stream << " " << index;
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::Call_indirect& instr){
        return stream << "call_indirect " << instr.tableidx << " " << instr.typeidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Ref_null& instr){
        stream << "ref.null ";
        switch(instr.heaptype){
            case RefType::funcref:
                return stream << "func";
            case RefType::externref:
                return stream << "extern";
            default:
                return stream;
        }
    }
    std::ostream& operator()(const WasmVM::Instr::Select& instr){
        stream << "select";
        if(!instr.valtypes.empty()){
            stream << " (result";
            for(ValueType type : instr.valtypes){
                stream << " " << type;
            }
            stream << ")";
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::Table_copy& instr){
        return stream << "table.copy " << instr.dstidx << " " << instr.srcidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Table_init& instr){
        return stream << "table.init " << instr.tableidx << " " << instr.elemidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Memory_copy& instr){
        return stream << "memory.copy " << instr.dstidx << " " << instr.srcidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Memory_init& instr){
        return stream << "memory.init " << instr.memidx << " " << instr.dataidx;
    }
    std::ostream& operator()(const WasmVM::Instr::I32_const& instr){
        return stream << "i32.const " << instr.value;
    }  
    std::ostream& operator()(const WasmVM::Instr::I64_const& instr){
        return stream << "i64.const " << instr.value;
    }
    std::ostream& operator()(const WasmVM::Instr::F32_const& instr){
        stream << "f32.const ";
        if(std::isnan(instr.value)){
            f32_t value = instr.value;
            u32_t uvalue = *(u32_t*)(&value);
            if(uvalue & 0x80000000UL){
                stream << "-";
            }
            stream << "nan";
            u32_t mantissa = uvalue & 0x7fffffUL;
            if(mantissa != 0x400000UL){
                auto old_fmt = stream.flags();
                stream << ":0x" << std::hex << mantissa;
                stream.flags(old_fmt);
                
            }
        }else{
            stream << instr.value;
        }
        return stream;
    }
    std::ostream& operator()(const WasmVM::Instr::F64_const& instr){
        stream << "f64.const ";
        if(std::isnan(instr.value)){
            f64_t value = instr.value;
            u64_t uvalue = *reinterpret_cast<u64_t*>(&value);
            if(uvalue & 0x8000000000000000ULL){
                stream << "-";
            }
            stream << "nan";
            u64_t mantissa = uvalue & 0xfffffffffffffULL;
            if(mantissa != 0x8000000000000ULL){
                auto old_fmt = stream.flags();
                stream << ":0x" << std::hex << mantissa;
                stream.flags(old_fmt);
            }
        }else{
            stream << instr.value;
        }
        return stream;
    }
    

#undef AtomicInstr
#undef OneIndexInstr
#undef MemoryInstr

private:
    std::ostream& stream;
};

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
std::ostream& WasmVM::operator<<(std::ostream& stream, const ConstInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
