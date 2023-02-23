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

#undef AtomicInstr
#undef OneIndexInstr

private:
    std::ostream& stream;
};

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
