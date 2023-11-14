// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Linker.hpp"

#include <fstream>
#include <ranges>
#include <Util.hpp>
#include <exception.hpp>

using namespace WasmVM;

Linker::Linker(Config config): config(config){}

void Linker::consume(std::filesystem::path module_path, std::istream& stream){
    // Decode
    WasmModule module = module_decode(stream);

    // Create module entry
    index_t module_index = modules.size();
    ModuleEntry& module_entry = modules.emplace_back();
    module_entry.path = module_path;

    // Types
    for(FuncType& type : module.types){
        if(type_map.contains(type)){
            module_entry.types.emplace_back(type_map[type]);
        }else{
            index_t typeidx = (index_t)output.types.size();
            type_map[type] = typeidx;
            output.types.emplace_back(type);
            module_entry.types.emplace_back(typeidx);
        }
    }

    // Imports
    for(WasmImport& import : module.imports){
        // Update explicit import
        if(!(import_map.contains(import.module) && import_map[import.module].contains(import.name))
            && (config.explicit_imports.contains(import.module) && config.explicit_imports[import.module].contains(import.name))
        ){
            std::visit(overloaded {
                [&](index_t& idx){
                    idx = type_map[module.types[idx]];
                    import_map[import.module][import.name] = import_counter.func++;
                },
                [&](TableType){
                    import_map[import.module][import.name] = import_counter.table++;
                },
                [&](MemType){
                    import_map[import.module][import.name] = import_counter.mem++;
                },
                [&](GlobalType){
                    import_map[import.module][import.name] = import_counter.global++;
                },
            }, import.desc);
            output.imports.emplace_back(import);
        }
        if(import_map.contains(import.module) && import_map[import.module].contains(import.name)){
            IndexEntry entry {
                .index = import_map[import.module][import.name],
                .kind = IndexEntry::Import
            };
            std::visit(overloaded {
                [&](index_t& idx){
                    idx = type_map[module.types[idx]];
                    module_entry.funcs.emplace_back(entry);
                },
                [&](TableType){
                    module_entry.tables.emplace_back(entry);
                },
                [&](MemType){
                    module_entry.mems.emplace_back(entry);
                },
                [&](GlobalType){
                    module_entry.globals.emplace_back(entry);
                },
            }, import.desc);
        }else{
            std::visit(overloaded {
                [&](index_t& idx){
                    idx = type_map[module.types[idx]];
                    module_entry.funcs.emplace_back(import);
                },
                [&](TableType){
                    module_entry.tables.emplace_back(import);
                },
                [&](MemType){
                    module_entry.mems.emplace_back(import);
                },
                [&](GlobalType){
                    module_entry.globals.emplace_back(import);
                },
            }, import.desc);
        }
    }

    // Funcs
    for(WasmFunc& func : module.funcs){
        module_entry.funcs.emplace_back(IndexEntry {
            .index = (index_t)output.funcs.size(),
            .kind = IndexEntry::Address
        });
        func.typeidx = type_map[module.types[func.typeidx]];
        module_index_list.funcs.emplace_back(module_index);
        output.funcs.emplace_back(func);
    }

    // Tables
    for(TableType& table : module.tables){
        module_entry.tables.emplace_back(IndexEntry {
            .index = (index_t)output.tables.size(),
            .kind = IndexEntry::Address
        });
        output.tables.emplace_back(table);
    }
    
    // Mems
    for(MemType& mem : module.mems){
        module_entry.mems.emplace_back(IndexEntry {
            .index = (index_t)output.mems.size(),
            .kind = IndexEntry::Address
        });
        output.mems.emplace_back(mem);
    }
    
    // Globals
    for(WasmGlobal& global : module.globals){
        module_entry.globals.emplace_back(IndexEntry {
            .index = (index_t)output.globals.size(),
            .kind = IndexEntry::Address
        });
        module_index_list.globals.emplace_back(module_index);
        output.globals.emplace_back(global);
    }

    // Elems
    for(WasmElem& elem : module.elems){
        module_entry.elems.emplace_back((index_t)output.elems.size());
        module_index_list.elems.emplace_back(module_index);
        output.elems.emplace_back(elem);
    }
    
    // Datas
    for(WasmData& data : module.datas){
        module_entry.datas.emplace_back((index_t)output.datas.size());
        module_index_list.datas.emplace_back(module_index);
        output.datas.emplace_back(data);
    }
    
    // Start
    module_entry.start = module.start;

    // Exports
    module_entry.exports.insert(module_entry.exports.begin(), module.exports.begin(), module.exports.end());
}

#define trace_extern_entries(DESCS) \
for(index_t desc_idx = 0; desc_idx < modules[module_idx].DESCS.size(); ++desc_idx){ \
    /* Already resolved */ \
    if(!std::holds_alternative<ExternEntry>(modules[module_idx].DESCS[desc_idx])){ \
        continue; \
    } \
    /* Traverse import dependencies */ \
    std::unordered_set<ExternEntry> traversed; \
    ExternEntry cur {module_idx, desc_idx}; \
    while(std::holds_alternative<ExternEntry>(modules[cur.module].DESCS[cur.address])){ \
        if(traversed.contains(cur)){ \
            throw Exception::Exception(std::string("undefined loop reference")); \
        } \
        traversed.emplace(cur); \
        cur = std::get<ExternEntry>(modules[cur.module].DESCS[cur.address]); \
    } \
    /* Update traversed entries */ \
    IndexEntry index = std::get<IndexEntry>(modules[cur.module].DESCS[cur.address]); \
    for(ExternEntry ext : traversed) { \
        modules[ext.module].DESCS[ext.address] = index; \
    } \
}

#define update_index(INDEX, DESC) { \
    if(INDEX >= module_entry.DESC ## s.size()){ \
        throw Exception::Exception(# DESC " index not found"); \
    } \
    IndexEntry& index_entry = std::get<IndexEntry>(module_entry.DESC ## s[INDEX]); \
    if(index_entry.kind == IndexEntry::Address){ \
        INDEX = index_entry.index + import_counter.DESC; \
    }else{ \
        INDEX = index_entry.index; \
    } \
}

WasmModule Linker::get(){
    /** Resolve pending imports **/
    // Create export map
    std::unordered_map<std::string, std::unordered_map<std::string, ExternEntry>> export_map;
    for(index_t module_idx = 0; module_idx < modules.size(); ++module_idx){
        ModuleEntry& moduleEntry = modules[module_idx];
        for(auto export_ : moduleEntry.exports){
            export_map[moduleEntry.path][export_.name] = ExternEntry {module_idx, export_.index};
        }
    }
    // Resolve imports
    for(ModuleEntry& module_entry : modules){
        resolve_imports(export_map, module_entry.funcs);
        resolve_imports(export_map, module_entry.tables);
        resolve_imports(export_map, module_entry.mems);
        resolve_imports(export_map, module_entry.globals);
    }
    // Trace exports
    for(index_t module_idx = 0; module_idx < modules.size(); ++module_idx){
        trace_extern_entries(funcs)
        trace_extern_entries(tables)
        trace_extern_entries(mems)
        trace_extern_entries(globals)
    }

    /** Update indices **/
    // Funcs
    for(index_t func_idx = 0; func_idx < output.funcs.size(); ++func_idx){
        WasmFunc& func = output.funcs[func_idx];
        for(WasmInstr& instr : func.body){
            instr_update_indices(instr, modules[module_index_list.funcs[func_idx]]);
        }
    }
    // Globals
    for(index_t global_idx = 0; global_idx < output.globals.size(); ++global_idx){
        WasmGlobal& global = output.globals[global_idx];
        instr_update_indices(global.init, modules[module_index_list.globals[global_idx]]);
    }
    // Elems
    for(index_t elem_idx = 0; elem_idx < output.elems.size(); ++elem_idx){
        WasmElem& elem = output.elems[elem_idx];
        ModuleEntry& module_entry = modules[module_index_list.elems[elem_idx]];
        for(ConstInstr& instr : elem.elemlist){
            instr_update_indices(instr, module_entry);
        }
        IndexEntry& index_entry = std::get<IndexEntry>(module_entry.tables[elem.mode.tableidx.value_or(0)]);
        if(index_entry.kind == IndexEntry::Address){
            elem.mode.tableidx = index_entry.index + import_counter.table;
        }else{
            elem.mode.tableidx = index_entry.index;
        }
        if(elem.mode.offset){
            instr_update_indices(elem.mode.offset.value(), module_entry);
        }
    }
    // Datas
    for(index_t data_idx = 0; data_idx < output.datas.size(); ++data_idx){
        WasmData& data = output.datas[data_idx];
        ModuleEntry& module_entry = modules[module_index_list.datas[data_idx]];
        IndexEntry& index_entry = std::get<IndexEntry>(module_entry.mems[data.mode.memidx.value_or(0)]);
        if(index_entry.kind == IndexEntry::Address){
            data.mode.memidx = index_entry.index + import_counter.mem;
        }else{
            data.mode.memidx = index_entry.index;
        }
        if(data.mode.offset){
            instr_update_indices(data.mode.offset.value(), module_entry);
        }
    }

    /** Starts **/
    compose_start_funcs();

    /** Exports **/
    for(auto export_pair : config.explicit_exports){
        std::filesystem::path export_path = export_pair.first;
        if(std::filesystem::exists(export_path)){
            export_path = std::filesystem::canonical(export_path);
        }
        for(ModuleEntry& module_entry : modules){
            if(export_pair.first == module_entry.path.string() || export_path == module_entry.path){
                for(WasmExport export_ : export_pair.second){
                    switch(export_.desc){
                        case WasmExport::DescType::func :
                            update_index(export_.index, func)
                        break;
                        case WasmExport::DescType::table :
                            update_index(export_.index, table)
                        break;
                        case WasmExport::DescType::mem :
                            update_index(export_.index, mem)
                        break;
                        case WasmExport::DescType::global :
                            update_index(export_.index, global)
                        break;
                    }
                    output.exports.emplace_back(export_);
                }
            }
        }
    }
    return output;
}

void Linker::resolve_imports(std::unordered_map<std::string, std::unordered_map<std::string, ExternEntry>> &export_map, std::vector<Descriptor> &descs){
    for(Descriptor& desc : descs){
        if(std::holds_alternative<WasmImport>(desc)){
            WasmImport import = std::get<WasmImport>(desc);
            if(import_map.contains(import.module) && import_map[import.module].contains(import.name)){
                // Existing imports
                desc.emplace<IndexEntry>(IndexEntry {
                    .index = import_map[import.module][import.name],
                    .kind = IndexEntry::Import
                });
            }else if(export_map.contains(import.module) && export_map[import.module].contains(import.name)){
                // Existing exports (direct)
                desc.emplace<ExternEntry>(export_map[import.module][import.name]);
            }else{
                std::filesystem::path module_path = import.module;
                if(std::filesystem::exists(module_path)){
                    module_path = std::filesystem::canonical(module_path);
                }
                if(export_map.contains(module_path) && export_map[module_path].contains(import.name)){
                    // Existing exports (canonical path)
                    desc.emplace<ExternEntry>(export_map[module_path][import.name]);
                }else{
                    // Create implicit import
                    index_t index = std::visit(overloaded {
                        [&](index_t& idx){
                            return import_counter.func++;
                        },
                        [&](TableType){
                            return import_counter.table++;
                        },
                        [&](MemType){
                            return import_counter.mem++;
                        },
                        [&](GlobalType){
                            return import_counter.global++;
                        },
                    }, import.desc);
                    desc.emplace<IndexEntry>(IndexEntry {
                        .index = index,
                        .kind = IndexEntry::Import
                    });
                    import_map[import.module][import.name] = index;
                    output.imports.emplace_back(import);
                }
            }
        }
    }
}

std::size_t std::hash<WasmVM::Linker::ExternEntry>::operator()(const WasmVM::Linker::ExternEntry& entry) const {
    return std::hash<index_t>{}(entry.address) ^ std::hash<index_t>{}(entry.module);
}

bool std::equal_to<WasmVM::Linker::ExternEntry>::operator()(const WasmVM::Linker::ExternEntry& a, const WasmVM::Linker::ExternEntry& b) const{
    return (a.address == b.address) && (a.module == b.module);
}

void Linker::instr_update_indices(WasmInstr& instr, ModuleEntry& module_entry){
    std::visit(overloaded {
        [](auto&){},
        [&](Instr::Call& ins){
            update_index(ins.index, func)
        },
        [&](Instr::Block& ins){
            if(ins.type){
                ins.type = module_entry.types[ins.type.value()];
            }
        },
        [&](Instr::Loop& ins){
            if(ins.type){
                ins.type = module_entry.types[ins.type.value()];
            }
        },
        [&](Instr::If& ins){
            if(ins.type){
                ins.type = module_entry.types[ins.type.value()];
            }
        },
        [&](Instr::Call_indirect& ins){
            ins.typeidx = module_entry.types[ins.typeidx];
            update_index(ins.tableidx, table)
        },
        [&](Instr::Ref_func& ins){
            update_index(ins.index, func)
        },
        [&](Instr::Global_get& ins){
            update_index(ins.index, global)
        },
        [&](Instr::Global_set& ins){
            update_index(ins.index, global)
        },
        [&](Instr::Table_get& ins){
            update_index(ins.index, table)
        },
        [&](Instr::Table_set& ins){
            update_index(ins.index, table)
        },
        [&](Instr::Table_size& ins){
            update_index(ins.index, table)
        },
        [&](Instr::Table_grow& ins){
            update_index(ins.index, table)
        },
        [&](Instr::Table_fill& ins){
            update_index(ins.index, table)
        },
        [&](Instr::Table_copy& ins){
            update_index(ins.dstidx, table)
            update_index(ins.srcidx, table)
        },
        [&](Instr::Table_init& ins){
            update_index(ins.tableidx, table)
            ins.elemidx = module_entry.elems[ins.elemidx];
        },
        [&](Instr::Elem_drop& ins){
            ins.index = module_entry.elems[ins.index];
        },
        [&](Instr::I32_load& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::F32_load& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::F64_load& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_load8_s& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_load8_u& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_load16_s& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_load16_u& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load8_s& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load8_u& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load16_s& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load16_u& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load32_s& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_load32_u& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_store& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_store& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::F32_store& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::F64_store& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_store8& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I32_store16& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_store8& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_store16& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::I64_store32& ins){
            update_index(ins.memidx, mem)
        },
        [&](Instr::Memory_size& ins){
            update_index(ins.index, mem)
        },
        [&](Instr::Memory_grow& ins){
            update_index(ins.index, mem)
        },
        [&](Instr::Memory_fill& ins){
            update_index(ins.index, mem)
        },
        [&](Instr::Memory_copy& ins){
            update_index(ins.dstidx, mem)
            update_index(ins.srcidx, mem)
        },
        [&](Instr::Memory_init& ins){
            update_index(ins.memidx, mem)
            ins.dataidx = module_entry.datas[ins.dataidx];
        },
        [&](Instr::Data_drop& ins){
            ins.index = module_entry.datas[ins.index];
        }
    }, instr);
}

void Linker::instr_update_indices(ConstInstr& instr, ModuleEntry& module_entry){
    std::visit(overloaded {
        [](auto&){},
        [&](Instr::Ref_func& ins){
            update_index(ins.index, func)
        },
        [&](Instr::Global_get& ins){
            update_index(ins.index, global)
        }
    }, instr);
}

void Linker::compose_start_funcs(){
    // Collect start functions
    std::vector<index_t> func_indices;
    std::visit(overloaded {
        [&](Config::StartMode& mode){
            if(mode == Config::StartMode::All){
                for(ModuleEntry& module_entry : modules){
                    if(module_entry.start){
                        index_t index = module_entry.start.value();
                        update_index(index, func)
                        func_indices.emplace_back(index);
                    }
                }
            }
        },
        [&](std::vector<Config::StartEntry>& entries){
            for(Config::StartEntry& entry : entries){
                std::filesystem::path start_path = entry.first;
                if(std::filesystem::exists(start_path)){
                    start_path = std::filesystem::canonical(start_path);
                }
                for(ModuleEntry& module_entry : modules){
                    if(entry.first == module_entry.path || start_path == module_entry.path){
                        if(!entry.second && !module_entry.start){
                            throw Exception::Exception("start section not exists in explicit start entry");
                        }
                        index_t index;
                        if(entry.second){
                            index = entry.second.value();
                        }else{
                            index = module_entry.start.value();
                        }
                        update_index(index, func)
                        func_indices.emplace_back(index);
                    }
                }
            }
        }
    }, config.start_func);

    // Compose
    if(func_indices.size() > 0){
        if(func_indices.size() > 1){
            output.start.emplace<index_t>(output.funcs.size() + import_counter.func);
            WasmFunc& start_func = output.funcs.emplace_back();
            FuncType type;
            if(type_map.contains(type)){
                start_func.typeidx = type_map[type];
            }else{
                start_func.typeidx = output.types.size();
                output.types.emplace_back(type);
            }
            for(index_t index : func_indices){
                start_func.body.emplace_back<Instr::Call>(Instr::Call(index));
            }
            start_func.body.emplace_back(Instr::End());
        }else{
            output.start.emplace(func_indices[0]);
        }
    }
}