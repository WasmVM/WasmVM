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
    AtomicInstr(WasmVM::Instr::Throw_ref, "throw_ref")
    AtomicInstr(WasmVM::Instr::Ref_is_null, "ref.is_null")
    AtomicInstr(WasmVM::Instr::Ref_eq, "ref.eq")
    AtomicInstr(WasmVM::Instr::Ref_as_non_null, "ref.as_non_null")
    AtomicInstr(WasmVM::Instr::Ref_i31, "ref.i31")
    AtomicInstr(WasmVM::Instr::I31_get_s, "i31.get_s")
    AtomicInstr(WasmVM::Instr::I31_get_u, "i31.get_u")
    AtomicInstr(WasmVM::Instr::Any_convert_extern, "any.convert_extern")
    AtomicInstr(WasmVM::Instr::Extern_convert_any, "extern.convert_any")
    AtomicInstr(WasmVM::Instr::Array_len, "array.len")

    OneIndexInstr(WasmVM::Instr::Struct_new, "struct.new")
    OneIndexInstr(WasmVM::Instr::Struct_new_default, "struct.new_default")
    OneIndexInstr(WasmVM::Instr::Array_new, "array.new")
    OneIndexInstr(WasmVM::Instr::Array_new_default, "array.new_default")
    OneIndexInstr(WasmVM::Instr::Array_get, "array.get")
    OneIndexInstr(WasmVM::Instr::Array_get_s, "array.get_s")
    OneIndexInstr(WasmVM::Instr::Array_get_u, "array.get_u")
    OneIndexInstr(WasmVM::Instr::Array_set, "array.set")
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
    AtomicInstr(WasmVM::Instr::I32_trunc_f32_s, "i32.trunc_f32_s")
    AtomicInstr(WasmVM::Instr::I32_trunc_f32_u, "i32.trunc_f32_u")
    AtomicInstr(WasmVM::Instr::I32_trunc_f64_s, "i32.trunc_f64_s")
    AtomicInstr(WasmVM::Instr::I32_trunc_f64_u, "i32.trunc_f64_u")
    AtomicInstr(WasmVM::Instr::I64_extend_i32_s, "i64.extend_s_i32")
    AtomicInstr(WasmVM::Instr::I64_extend_i32_u, "i64.extend_u_i32")
    AtomicInstr(WasmVM::Instr::I64_trunc_f32_s, "i64.trunc_f32_s")
    AtomicInstr(WasmVM::Instr::I64_trunc_f32_u, "i64.trunc_f32_u")
    AtomicInstr(WasmVM::Instr::I64_trunc_f64_s, "i64.trunc_f64_s")
    AtomicInstr(WasmVM::Instr::I64_trunc_f64_u, "i64.trunc_f64_u")
    AtomicInstr(WasmVM::Instr::F32_convert_i32_s, "f32.convert_s_i32")
    AtomicInstr(WasmVM::Instr::F32_convert_i32_u, "f32.convert_u_i32")
    AtomicInstr(WasmVM::Instr::F32_convert_i64_s, "f32.convert_s_i64")
    AtomicInstr(WasmVM::Instr::F32_convert_i64_u, "f32.convert_u_i64")
    AtomicInstr(WasmVM::Instr::F32_demote_f64, "f32.demote_f64")
    AtomicInstr(WasmVM::Instr::F64_convert_i32_s, "f64.convert_s_i32")
    AtomicInstr(WasmVM::Instr::F64_convert_i32_u, "f64.convert_u_i32")
    AtomicInstr(WasmVM::Instr::F64_convert_i64_s, "f64.convert_s_i64")
    AtomicInstr(WasmVM::Instr::F64_convert_i64_u, "f64.convert_u_i64")
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

    OneIndexInstr(WasmVM::Instr::Throw, "throw")
    OneIndexInstr(WasmVM::Instr::Call, "call")
    OneIndexInstr(WasmVM::Instr::Call_ref, "call_ref")
    OneIndexInstr(WasmVM::Instr::Return_call, "return_call")
    OneIndexInstr(WasmVM::Instr::Return_call_ref, "return_call_ref")
    OneIndexInstr(WasmVM::Instr::Br, "br")
    OneIndexInstr(WasmVM::Instr::Br_if, "br_if")
    OneIndexInstr(WasmVM::Instr::Br_on_null, "br_on_null")
    OneIndexInstr(WasmVM::Instr::Br_on_non_null, "br_on_non_null")
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

    std::ostream& operator()(const WasmVM::Instr::Try_table& instr){
        stream << "try_table";
        if(instr.type){
            stream << " (type " << instr.type.value() << ")";
        }
        for(const WasmVM::Instr::TryCatchEntry& ce : instr.catches){
            switch(ce.kind){
                case WasmVM::Instr::TryCatchEntry::Catch:
                    stream << " (catch " << ce.tag_idx << " " << ce.label_idx << ")";
                break;
                case WasmVM::Instr::TryCatchEntry::CatchRef:
                    stream << " (catch_ref " << ce.tag_idx << " " << ce.label_idx << ")";
                break;
                case WasmVM::Instr::TryCatchEntry::CatchAll:
                    stream << " (catch_all " << ce.label_idx << ")";
                break;
                case WasmVM::Instr::TryCatchEntry::CatchAllRef:
                    stream << " (catch_all_ref " << ce.label_idx << ")";
                break;
            }
        }
        return stream;
    }
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
    std::ostream& operator()(const WasmVM::Instr::Return_call_indirect& instr){
        return stream << "return_call_indirect " << instr.tableidx << " " << instr.typeidx;
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
    std::ostream& operator()(const WasmVM::Instr::Ref_test& instr){
        return stream << "ref.test " << instr.heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Ref_test_null& instr){
        return stream << "ref.test null " << instr.heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Ref_cast& instr){
        return stream << "ref.cast " << instr.heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Ref_cast_null& instr){
        return stream << "ref.cast null " << instr.heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Br_on_cast& instr){
        return stream << "br_on_cast " << instr.labelidx << " " << instr.src_heaptype << " " << instr.dst_heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Br_on_cast_fail& instr){
        return stream << "br_on_cast_fail " << instr.labelidx << " " << instr.src_heaptype << " " << instr.dst_heaptype;
    }
    std::ostream& operator()(const WasmVM::Instr::Struct_get& instr){
        return stream << "struct.get " << instr.typeidx << " " << instr.fieldidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Struct_get_s& instr){
        return stream << "struct.get_s " << instr.typeidx << " " << instr.fieldidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Struct_get_u& instr){
        return stream << "struct.get_u " << instr.typeidx << " " << instr.fieldidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Struct_set& instr){
        return stream << "struct.set " << instr.typeidx << " " << instr.fieldidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_new_fixed& instr){
        return stream << "array.new_fixed " << instr.typeidx << " " << instr.n;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_new_data& instr){
        return stream << "array.new_data " << instr.typeidx << " " << instr.dataidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_new_elem& instr){
        return stream << "array.new_elem " << instr.typeidx << " " << instr.elemidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_fill& instr){
        return stream << "array.fill " << instr.typeidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_copy& instr){
        return stream << "array.copy " << instr.dst_typeidx << " " << instr.src_typeidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_init_data& instr){
        return stream << "array.init_data " << instr.typeidx << " " << instr.dataidx;
    }
    std::ostream& operator()(const WasmVM::Instr::Array_init_elem& instr){
        return stream << "array.init_elem " << instr.typeidx << " " << instr.elemidx;
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
    InstrVisitor v(stream);
#define ATOM(OP,T) case Opcode::OP: { Instr::T t; return v(t); }
#define ONE(OP,T)  case Opcode::OP: { auto& _o=std::get<WasmInstr::OneIdx>(instr.imm); Instr::T t{_o.index}; return v(t); }
#define TWO(OP,T,A,B) case Opcode::OP: { auto& _tw=std::get<WasmInstr::TwoIdx>(instr.imm); Instr::T t{_tw.A,_tw.B}; return v(t); }
#define BLK(OP,T)  case Opcode::OP: { auto& _b=std::get<WasmInstr::BlockType>(instr.imm); Instr::T t{_b.type}; return v(t); }
#define MEM(OP,T)  case Opcode::OP: { auto& _m=std::get<WasmInstr::MemArg>(instr.imm); Instr::T t{_m.memidx,_m.offset,_m.align}; return v(t); }
    switch(instr.opcode) {
    ATOM(Unreachable,Unreachable) ATOM(Nop,Nop) ATOM(Else,Else) ATOM(End,End)
    ATOM(Return,Return) ATOM(Throw_ref,Throw_ref)
    ATOM(Ref_is_null,Ref_is_null) ATOM(Ref_eq,Ref_eq) ATOM(Ref_as_non_null,Ref_as_non_null)
    ATOM(Ref_i31,Ref_i31) ATOM(I31_get_s,I31_get_s) ATOM(I31_get_u,I31_get_u)
    ATOM(Any_convert_extern,Any_convert_extern) ATOM(Extern_convert_any,Extern_convert_any)
    ATOM(Array_len,Array_len) ATOM(Drop,Drop)
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
    BLK(Block,Block) BLK(Loop,Loop) BLK(If,If)
    ONE(Throw,Throw) ONE(Call,Call) ONE(Call_ref,Call_ref)
    ONE(Return_call,Return_call) ONE(Return_call_ref,Return_call_ref)
    ONE(Br,Br) ONE(Br_if,Br_if) ONE(Br_on_null,Br_on_null) ONE(Br_on_non_null,Br_on_non_null)
    ONE(Ref_func,Ref_func)
    ONE(Local_get,Local_get) ONE(Local_set,Local_set) ONE(Local_tee,Local_tee)
    ONE(Global_get,Global_get) ONE(Global_set,Global_set)
    ONE(Table_get,Table_get) ONE(Table_set,Table_set)
    ONE(Table_size,Table_size) ONE(Table_grow,Table_grow) ONE(Table_fill,Table_fill)
    ONE(Elem_drop,Elem_drop)
    ONE(Memory_size,Memory_size) ONE(Memory_grow,Memory_grow)
    ONE(Memory_fill,Memory_fill) ONE(Data_drop,Data_drop)
    ONE(Struct_new,Struct_new) ONE(Struct_new_default,Struct_new_default)
    ONE(Array_new,Array_new) ONE(Array_new_default,Array_new_default)
    ONE(Array_get,Array_get) ONE(Array_get_s,Array_get_s) ONE(Array_get_u,Array_get_u) ONE(Array_set,Array_set)
    ONE(Array_fill,Array_fill)
    TWO(Call_indirect,Call_indirect,a,b)       // a=tableidx, b=typeidx
    TWO(Return_call_indirect,Return_call_indirect,a,b)
    TWO(Struct_get,Struct_get,a,b) TWO(Struct_get_s,Struct_get_s,a,b)
    TWO(Struct_get_u,Struct_get_u,a,b) TWO(Struct_set,Struct_set,a,b)
    TWO(Array_new_fixed,Array_new_fixed,a,b) TWO(Array_new_data,Array_new_data,a,b)
    TWO(Array_new_elem,Array_new_elem,a,b) TWO(Array_copy,Array_copy,a,b)
    TWO(Array_init_data,Array_init_data,a,b) TWO(Array_init_elem,Array_init_elem,a,b)
    TWO(Table_copy,Table_copy,a,b) TWO(Table_init,Table_init,a,b)
    TWO(Memory_copy,Memory_copy,a,b) TWO(Memory_init,Memory_init,a,b)
    MEM(I32_load,I32_load) MEM(I64_load,I64_load) MEM(F32_load,F32_load) MEM(F64_load,F64_load)
    MEM(I32_load8_s,I32_load8_s) MEM(I32_load8_u,I32_load8_u)
    MEM(I32_load16_s,I32_load16_s) MEM(I32_load16_u,I32_load16_u)
    MEM(I64_load8_s,I64_load8_s) MEM(I64_load8_u,I64_load8_u)
    MEM(I64_load16_s,I64_load16_s) MEM(I64_load16_u,I64_load16_u)
    MEM(I64_load32_s,I64_load32_s) MEM(I64_load32_u,I64_load32_u)
    MEM(I32_store,I32_store) MEM(I64_store,I64_store) MEM(F32_store,F32_store) MEM(F64_store,F64_store)
    MEM(I32_store8,I32_store8) MEM(I32_store16,I32_store16)
    MEM(I64_store8,I64_store8) MEM(I64_store16,I64_store16) MEM(I64_store32,I64_store32)
    case Opcode::I32_const: { auto& c=std::get<WasmInstr::ConstI32>(instr.imm); Instr::I32_const t{c.value}; return v(t); }
    case Opcode::I64_const: { auto& c=std::get<WasmInstr::ConstI64>(instr.imm); Instr::I64_const t{c.value}; return v(t); }
    case Opcode::F32_const: { auto& c=std::get<WasmInstr::ConstF32>(instr.imm); Instr::F32_const t{c.value}; return v(t); }
    case Opcode::F64_const: { auto& c=std::get<WasmInstr::ConstF64>(instr.imm); Instr::F64_const t{c.value}; return v(t); }
    case Opcode::Ref_null: { auto& h=std::get<WasmInstr::HeapRef>(instr.imm); Instr::Ref_null t{h.heaptype}; return v(t); }
    case Opcode::Ref_test: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_test t{h.heaptype}; return v(t); }
    case Opcode::Ref_test_null: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_test_null t{h.heaptype}; return v(t); }
    case Opcode::Ref_cast: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_cast t{h.heaptype}; return v(t); }
    case Opcode::Ref_cast_null: { auto& h=std::get<WasmInstr::HeapI32>(instr.imm); Instr::Ref_cast_null t{h.heaptype}; return v(t); }
    case Opcode::Br_on_cast: { auto& c=std::get<WasmInstr::CastOp>(instr.imm); Instr::Br_on_cast t{c.label,c.src_n,c.dst_n,c.src_ht,c.dst_ht}; return v(t); }
    case Opcode::Br_on_cast_fail: { auto& c=std::get<WasmInstr::CastOp>(instr.imm); Instr::Br_on_cast_fail t{c.label,c.src_n,c.dst_n,c.src_ht,c.dst_ht}; return v(t); }
    case Opcode::Br_table: { auto& bt=std::get<WasmInstr::BrTab>(instr.imm); Instr::Br_table t; t.indices=bt.indices; return v(t); }
    case Opcode::Try_table: { auto& tt=std::get<WasmInstr::TryTab>(instr.imm); Instr::Try_table t; t.type=tt.type; t.catches=tt.catches; return v(t); }
    case Opcode::Select: { auto& sv=std::get<WasmInstr::SelectV>(instr.imm); Instr::Select t; t.valtypes=sv.valtypes; return v(t); }
    default: break;
    }
#undef ATOM
#undef ONE
#undef TWO
#undef BLK
#undef MEM
    return stream;
}
std::ostream& WasmVM::operator<<(std::ostream& stream, const ConstInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
