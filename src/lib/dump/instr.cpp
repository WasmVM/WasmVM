// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

using namespace WasmVM;

struct InstrVisitor {
    InstrVisitor(std::ostream& stream) : stream(stream){}

#define AtomicInstr(T, S) \
    std::ostream& operator()(T&){ \
        return stream << S; \
    }
#define OneIndexInstr(T, S) \
    std::ostream& operator()(T& instr){ \
        return stream << S << " " << instr.index; \
    }
#define MemoryInstr(T, S) \
    std::ostream& operator()(T& instr){ \
        return stream << S << " " << instr.memidx << " offset=" << (u64_t)instr.offset << " align=" << (u32_t)instr.align; \
    }

    AtomicInstr(WasmVM::Instr::Unreachable, "unreachable")
    AtomicInstr(WasmVM::Instr::Nop, "nop")
    AtomicInstr(WasmVM::Instr::Else, "else")
    AtomicInstr(WasmVM::Instr::End, "end")
    AtomicInstr(WasmVM::Instr::Return, "return")
    AtomicInstr(WasmVM::Instr::Ref_is_null, "ref.is_null")
    AtomicInstr(WasmVM::Instr::Drop, "drop")

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

    std::ostream& operator()(WasmVM::Instr::Block& instr){
        stream << "block";
        if(instr.type){
            stream << " " << instr.type.value();
        }
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Loop& instr){
        stream << "loop";
        if(instr.type){
            stream << " " << instr.type.value();
        }
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::If& instr){
        stream << "if";
        if(instr.type){
            stream << " " << instr.type.value();
        }
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Br_table& instr){
        stream << "br_table";
        for(index_t index : instr.indices){
            stream << " " << index;
        }
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Call_indirect& instr){
        return stream << "call_indirect " << instr.tableidx << " " << instr.typeidx;
    }
    std::ostream& operator()(WasmVM::Instr::Ref_null& instr){
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
    std::ostream& operator()(WasmVM::Instr::Select& instr){
        stream << "select";
        for(ValueType type : instr.valtypes){
            stream << " " << type;
        }
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Table_copy& instr){
        return stream << "table.copy " << instr.dstidx << " " << instr.srcidx;
    }
    std::ostream& operator()(WasmVM::Instr::Table_init& instr){
        return stream << "table.init " << instr.tableidx << " " << instr.elemidx;
    }
    std::ostream& operator()(WasmVM::Instr::Memory_copy& instr){
        return stream << "memory.copy " << instr.dstidx << " " << instr.srcidx;
    }
    std::ostream& operator()(WasmVM::Instr::Memory_init& instr){
        return stream << "memory.init " << instr.memidx << " " << instr.dataidx;
    }   

#undef AtomicInstr
#undef OneIndexInstr
#undef MemoryInstr

private:
    std::ostream& stream;
};

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
