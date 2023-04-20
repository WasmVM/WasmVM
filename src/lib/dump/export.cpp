// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmExport& export_){
    stream << "  (export " << export_.name;
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
    }
    stream << export_.index << ") )" << std::endl;
    return stream;
}