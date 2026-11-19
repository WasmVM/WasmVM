// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmElem& elem){
    return dump_elem(stream, elem, nullptr);
}

std::ostream& WasmVM::dump_elem(std::ostream& stream, const WasmElem& elem, const std::string* name){
    stream << "  (elem";
    dump_def_name(stream, name) << " ";
    switch(elem.mode.type){
        case WasmElem::ElemMode::Mode::active :
            stream << "(table ";
            dump_ref(stream, DumpSpace::table, elem.mode.tableidx.value()) << ") ";
            stream << "(offset " << elem.mode.offset.value() << ") ";
        break;
        case WasmElem::ElemMode::Mode::declarative :
            stream << "declare ";
        break;
        default:
        break;
    }
    stream << elem.type << " ";
    for(const ConstInstr& instr : elem.elemlist){
        stream << std::endl << "    (item " << instr << ")";
    }
    return stream << std::endl << "  )" << std::endl;
}