// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"
#include <Util.hpp>

#include <variant>

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmImport& import){
    return dump_import(stream, import, nullptr);
}

// `name` names the imported definition.
std::ostream& WasmVM::dump_import(std::ostream& stream, const WasmImport& import, const std::string* name){
    stream << "  (import \"" << import.module << "\" \"" << import.name << "\"";
    std::visit(overloaded {
        [&](index_t typeidx){
            stream << " (func";
            dump_def_name(stream, name) << " (type ";
            dump_ref(stream, DumpSpace::type, typeidx) << ") )";
        },
        [&](WasmVM::TagType tag){
            stream << " (tag";
            dump_def_name(stream, name) << " (type ";
            dump_ref(stream, DumpSpace::type, tag.typeidx) << ") )";
        },
        [&](WasmVM::TableType table){
            stream << " (table";
            dump_def_name(stream, name) << " " << table.limits.min;
            if(table.limits.max){
                stream << " " << table.limits.max.value();
            }
            stream << " " << table.elemtype << ")";
        },
        [&](WasmVM::MemType mem){
            stream << " (memory";
            dump_def_name(stream, name) << " ";
            if(mem.is64) stream << "i64 ";
            stream << mem.min;
            if(mem.max){
                stream << " " << mem.max.value();
            }
            stream << ")";
        },
        [&](WasmVM::GlobalType global){
            stream << " (global";
            dump_def_name(stream, name);
            if(global.mut == WasmVM::GlobalType::Mut::variable){
                stream << " (mut " << global.type << ") )";
            }else{
                stream << " " << global.type << " )";
            }
        }
    }, import.desc);
    stream << " )" << std::endl;
    return stream;
}