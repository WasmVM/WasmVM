// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmFunc& func){
    stream << "  (func (type " << func.typeidx << ")";
    for(ValueType local : func.locals){
        stream << " (local " << local << ")";
    }
    stream << std::endl;
    for(WasmInstr& instr : func.body){
        stream << "    " << instr << std::endl;
    }
    stream << "  )" << std::endl;
    return stream;
}