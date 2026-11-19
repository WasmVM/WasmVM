// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmExport& export_){
    stream << "  (export \"" << export_.name << "\"";
    switch(export_.desc){
        case WasmExport::DescType::func :
            stream << " (func ";
        break;
        case WasmExport::DescType::global :
            stream << " (global ";
        break;
        case WasmExport::DescType::mem :
            stream << " (mem ";
        break;
        case WasmExport::DescType::table :
            stream << " (table ";
        break;
        case WasmExport::DescType::tag :
            stream << " (tag ";
        break;
    }
    DumpSpace space = DumpSpace::func;
    switch(export_.desc){
        case WasmExport::DescType::func :   space = DumpSpace::func; break;
        case WasmExport::DescType::global : space = DumpSpace::global; break;
        case WasmExport::DescType::mem :    space = DumpSpace::mem; break;
        case WasmExport::DescType::table :  space = DumpSpace::table; break;
        case WasmExport::DescType::tag :    space = DumpSpace::tag; break;
    }
    dump_ref(stream, space, export_.index) << ") )" << std::endl;
    return stream;
}