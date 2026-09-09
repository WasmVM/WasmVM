/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "dump.hpp"

#include <algorithm>
#include <string>
#include <ostream>
#include <utility>
#include <vector>

using namespace WasmVM;

std::ostream& WasmVM::module_dump(const WasmModule& module, std::ostream& stream){
    stream << "(module " << std::endl;
    {
        // Types outside every explicit (rec ...) group print bare; a group
        // prints as one (rec ...) with its members indented.
        std::vector<RecGroup> recs(module.recs.begin(), module.recs.end());
        std::sort(recs.begin(), recs.end(), [](const RecGroup& a, const RecGroup& b){ return a.first < b.first; });
        size_t k = 0;
        for(index_t i = 0; i < module.types.size();){
            if(k < recs.size() && recs[k].first == i){
                stream << "  (rec" << std::endl;
                for(index_t j = 0; j < recs[k].count && (i + j) < module.types.size(); ++j){
                    dump_subtype(stream, module.types[i + j], "    ");
                }
                stream << "  )" << std::endl;
                i += recs[k].count;
                k += 1;
            }else{
                stream << module.types[i];
                i += 1;
            }
        }
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
    // Tags sit between the memories and the globals, matching the binary
    // section order. Imported tags were already printed by the import loop
    // above, so the module-defined ones keep their place in the index space.
    for(index_t typeidx : module.tags){
        stream << "  (tag (type " << typeidx << ") )" << std::endl;
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