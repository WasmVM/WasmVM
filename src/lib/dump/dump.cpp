/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "dump.hpp"

#include <string>
#include <sstream>
#include <utility>

using namespace WasmVM;

std::string WasmVM::module_dump(WasmModule& module){
    std::stringstream stream;
    stream << "(module " << std::endl;
    for(FuncType& type : module.types){
        stream << type;
    }
    for(WasmImport& import : module.imports){
        stream << import;
    }
    for(WasmFunc& func : module.funcs){
        stream << func;
    }
    for(TableType& table : module.tables){
        stream << table;
    }
    for(MemType& mem : module.mems){
        stream << mem;
    }
    for(WasmGlobal& global : module.globals){
        stream << global;
    }
    for(WasmExport& export_ : module.exports){
        stream << export_;
    }
    if(module.start){
        stream << "  (start " << module.start.value() << ")" << std::endl;
    }
    for(WasmElem& elem : module.elems){
        stream << elem;
    }
    for(WasmData& data : module.datas){
        stream << data;
    }
    stream << ")" << std::endl;
    return stream.str();
}