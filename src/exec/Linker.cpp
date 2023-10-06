// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Linker.hpp"

#include <fstream>
#include <ranges>
#include <Util.hpp>
#include <exception.hpp>

using namespace WasmVM;

void Linker::consume(std::filesystem::path module_path){
    // Decode
    std::ifstream module_file(module_path);
    WasmModule module = module_decode(module_file);
    module_file.close();

    // Create module entry
    ModuleEntry& entry = entries.emplace_back();
    entry.path = module_path;

    // Types
    for(FuncType& type : module.types){
        if(typemap.contains(type)){
            entry.types.emplace_back(typemap[type]);
        }else{
            index_t typeidx = (index_t)output.types.size();
            typemap[type] = typeidx;
            output.types.emplace_back(type);
            entry.types.emplace_back(typeidx);
        }
    }

    // Imports
    for(WasmImport import : module.imports){
        bool pending = false;
        
        if(import.module.starts_with("./") || import.module.starts_with("../") || import.module.starts_with("/")){
            std::filesystem::path import_path;
            // Extend path
            if(import.module.starts_with("/")){
                import_path = import.module;
            }else{
                import_path = entry.path.parent_path() / import.module;
            }
            // Check existance
            if(!std::filesystem::exists(import_path)){
                import_path.replace_extension(".wasm");
            }
            if(std::filesystem::exists(import_path)){
                import.module = std::filesystem::canonical(import_path);
            }else{
                throw Exception::Exception(std::string("import module '" ) + import.module + "' not found");
            }
            pending = true;
        }
        std::visit(overloaded {
            [&](index_t& idx){
                entry.counter.func += 1;
                idx = typemap[module.types[idx]];
                if(pending){
                    entry.funcs.emplace_back(import);
                }else{
                    entry.funcs.emplace_back(resolved[import.module][import.name]);
                }
            },
            [&](TableType){
                entry.counter.table += 1;
                // TODO:
            },
            [&](MemType){
                entry.counter.mem += 1;
                // TODO:
            },
            [&](GlobalType){
                entry.counter.global += 1;
                // TODO:
            },
        }, import.desc);
    }

    // Funcs
    for(WasmFunc& func : module.funcs){
        entry.funcs.emplace_back((index_t)output.funcs.size());
        func.typeidx = typemap[module.types[func.typeidx]];
        output.funcs.emplace_back(func);
    }

    // TODO: Tables
    // TODO: Mems
    // TODO: Globals
    // TODO: Elems
    // TODO: Datas
    // TODO: Start

    // Exports
    for(WasmExport& export_ : module.exports){
        std::pair<ExternVal::ExternType, index_t>& export_entry = entry.exports[export_.name];
        switch(export_.desc){
            case WasmExport::DescType::func:
                export_entry.first = ExternVal::ExternType::Func;
            break;
            case WasmExport::DescType::table:
                export_entry.first = ExternVal::ExternType::Table;
            break;
            case WasmExport::DescType::mem:
                export_entry.first = ExternVal::ExternType::Mem;
            break;
            case WasmExport::DescType::global:
                export_entry.first = ExternVal::ExternType::Global;
            break;
        }
        export_entry.second = export_.index;
    }
}

WasmModule Linker::get(){
    /** Resolve pending imports **/
    // Create export map
    std::unordered_map<std::string, std::unordered_map<std::string, ExternEntry>> export_map;
    for(size_t entryIdx = 0; entryIdx < entries.size(); ++entryIdx){
        ModuleEntry& entry = entries[entryIdx];
        for(auto export_ : entry.exports){
            export_map[entry.path][export_.first] = ExternEntry {entryIdx, export_.second.second};
        }
    }
    // Resolve imports
    for(size_t entryIdx = 0; entryIdx < entries.size(); ++entryIdx){
        ModuleEntry& entry = entries[entryIdx];
        for(IndexEntry& funcEntry : entry.funcs){
            if(std::holds_alternative<WasmImport>(funcEntry)){
                WasmImport import = std::get<WasmImport>(funcEntry);
                // Replace with extern entry if found
                if(export_map.contains(import.module) && export_map[import.module].contains(import.name)){
                    funcEntry.emplace<ExternEntry>(export_map[import.module][import.name]);
                }else{
                    throw Exception::Exception(std::string("undefined reference to '") + import.name + "'");
                }
            }
        }
    }
    // Trace exports

    // TODO: Fix indices
    // TODO: Compose starts
    return output;
}