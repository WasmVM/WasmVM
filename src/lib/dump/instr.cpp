// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

using namespace WasmVM;

struct InstrVisitor {
    InstrVisitor(std::ostream& stream) : stream(stream){}

    std::ostream& operator()(WasmVM::Instr::Unreachable&){
        return stream << "unreachable";
    }
    std::ostream& operator()(WasmVM::Instr::Nop&){
        return stream << "nop";
    }
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
    std::ostream& operator()(WasmVM::Instr::End&){
        return stream << "end";
    }

private:
    std::ostream& stream;
};

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmInstr& instr){
    return std::visit(InstrVisitor(stream), instr);
}
