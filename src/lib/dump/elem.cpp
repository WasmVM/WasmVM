// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmElem& elem){
    stream << "  (elem ";
    switch(elem.mode.type){
        case WasmElem::ElemMode::Mode::active :
            stream << "(table " << elem.mode.tableidx.value() << ") ";
            stream << "(offset " << elem.mode.offset.value() << ") ";
        break;
        case WasmElem::ElemMode::Mode::declarative :
            stream << "declare ";
        break;
        default:
        break;
    }
    switch(elem.type){
        case RefType::externref :
            stream << "externref ";
        break;
        case RefType::funcref :
            stream << "funcref ";
        break;
    }
    for(const ConstInstr& instr : elem.elemlist){
        stream << std::endl << "    (item " << instr << ")";
    }
    return stream << std::endl << "  )" << std::endl;
}