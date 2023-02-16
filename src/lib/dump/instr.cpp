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

    AtomicInstr(WasmVM::Instr::Unreachable, "unreachable")
    AtomicInstr(WasmVM::Instr::Nop, "nop")
    AtomicInstr(WasmVM::Instr::Else, "else")
    AtomicInstr(WasmVM::Instr::End, "end")
    AtomicInstr(WasmVM::Instr::Return, "return")

    std::ostream& operator()(WasmVM::Instr::Call& instr){
        return stream << "call " << instr.index;
    }
    std::ostream& operator()(WasmVM::Instr::Block& instr){
        stream << "block ";
        std::visit(overloaded {
            [&](index_t& idx) {
                stream << idx;
            },
            [&](std::optional<ValueType>& type) {
                if(type){
                    stream << type.value();
                }
            }
        }, instr.type);
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Loop& instr){
        stream << "loop ";
        std::visit(overloaded {
            [&](index_t& idx) {
                stream << idx;
            },
            [&](std::optional<ValueType>& type) {
                if(type){
                    stream << type.value();
                }
            }
        }, instr.type);
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::If& instr){
        stream << "if ";
        std::visit(overloaded {
            [&](index_t& idx) {
                stream << idx;
            },
            [&](std::optional<ValueType>& type) {
                if(type){
                    stream << type.value();
                }
            }
        }, instr.type);
        return stream;
    }
    std::ostream& operator()(WasmVM::Instr::Br& instr){
        return stream << "br " << instr.index;
    }
    std::ostream& operator()(WasmVM::Instr::Br_if& instr){
        return stream << "br_if " << instr.index;
    }
    std::ostream& operator()(WasmVM::Instr::Br_table& instr){
        stream << "br_table";
        for(index_t index : instr.indices){
            stream << " " << index;
        }
        return stream;
    }

#undef AtomicInstr

private:
    std::ostream& stream;
};

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
