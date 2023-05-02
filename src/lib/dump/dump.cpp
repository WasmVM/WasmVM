/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "dump.hpp"

#include <string>
#include <ostream>
#include <utility>

using namespace WasmVM;

std::ostream& WasmVM::module_dump(const WasmModule& module, std::ostream& stream){
    stream << "(module " << std::endl;
    for(const FuncType& type : module.types){
        stream << type;
    }
    for(const WasmImport& import : module.imports){
        stream << import;
    }
    for(const WasmFunc& func : module.funcs){
        stream << func;
    }
    for(const TableType& table : module.tables){
        stream << table;
    }
    for(const MemType& mem : module.mems){
        stream << mem;
    }
    for(const WasmGlobal& global : module.globals){
        stream << global;
    }
    for(const WasmExport& export_ : module.exports){
        stream << export_;
    }
    if(module.start){
        stream << "  (start " << module.start.value() << ")" << std::endl;
    }
    for(const WasmElem& elem : module.elems){
        stream << elem;
    }
    for(const WasmData& data : module.datas){
        stream << data;
    }
    stream << ")" << std::endl;
    return stream;
}