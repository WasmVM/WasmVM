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

void Linker::consume(std::filesystem::path module_path){
    // Decode
    std::ifstream module_file(module_path);
    WasmModule module = module_decode(module_file);
    module_file.close();

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
        bool pending = false;
        if(import.module.starts_with("./") || import.module.starts_with("../") || import.module.starts_with("/")){
            std::filesystem::path import_path;
            // Extend path
            if(import.module.starts_with("/")){
                import_path = import.module;
            }else{
                import_path = module_entry.path.parent_path() / import.module;
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
        }else if(!(import_map.contains(import.module) && import_map[import.module].contains(import.name))){
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

        std::visit(overloaded {
            [&](index_t& idx){
                idx = type_map[module.types[idx]];
                if(pending){
                    module_entry.funcs.emplace_back(import);
                }else{
                    module_entry.funcs.emplace_back(IndexEntry {
                        .index = import_map[import.module][import.name],
                        .kind = IndexEntry::Import
                    });
                }
            },
            [&](TableType){
                if(pending){
                    module_entry.tables.emplace_back(import);
                }else{
                    module_entry.tables.emplace_back(IndexEntry {
                        .index = import_map[import.module][import.name],
                        .kind = IndexEntry::Import
                    });
                }
            },
            [&](MemType){
                if(pending){
                    module_entry.mems.emplace_back(import);
                }else{
                    module_entry.mems.emplace_back(IndexEntry {
                        .index = import_map[import.module][import.name],
                        .kind = IndexEntry::Import
                    });
                }
            },
            [&](GlobalType){
                if(pending){
                    module_entry.globals.emplace_back(import);
                }else{
                    module_entry.globals.emplace_back(IndexEntry {
                        .index = import_map[import.module][import.name],
                        .kind = IndexEntry::Import
                    });
                }
            },
        }, import.desc);
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

    /** Starts **/
    std::visit(overloaded {
        // Empty
        [](std::monostate&){},
        // Explicit
        [&](std::pair<std::filesystem::path, index_t>& start_pair){
            explicit_start_func(start_pair.first, start_pair.second);
        },
        // Compose/merge
        [&](Config::StartMode& mode){
            if(mode == Config::Merge){
                merge_start_funcs();
            }else{
                compose_start_funcs();
            }
        },
    }, config.start_func);

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
    return output;
}

void Linker::resolve_imports(std::unordered_map<std::string, std::unordered_map<std::string, ExternEntry>> &export_map, std::vector<Descriptor> &descs){
    for(Descriptor& desc : descs){
        if(std::holds_alternative<WasmImport>(desc)){
            WasmImport import = std::get<WasmImport>(desc);
            // Replace with extern entry if found
            if(export_map.contains(import.module) && export_map[import.module].contains(import.name)){
                desc.emplace<ExternEntry>(export_map[import.module][import.name]);
            }else{
                throw Exception::Exception(std::string("undefined reference to '") + import.name + "'");
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

}

void Linker::merge_start_funcs(){

}

void Linker::explicit_start_func(std::filesystem::path module_path, index_t index){
    for(ModuleEntry& module_entry : modules){
        // Module path
        if(module_path == module_entry.path){
            // Function index
            if(index >= module_entry.funcs.size()){
                throw Exception::Exception("explicit start function not found");
            }
            update_index(index, func)
            // Check type
            FuncType type;
            if(index < import_counter.func){
                index_t count = 0;
                bool found = false;
                for(WasmImport& import : output.imports){
                    if(std::holds_alternative<index_t>(import.desc)){
                        if(count++ == index){
                            type = output.types[std::get<index_t>(import.desc)];
                            found = true;
                            break;
                        }
                    }
                }
                if(!found){
                    throw Exception::Exception("explicit start type not found");
                }
            }else{
                type = output.types[output.funcs[index - import_counter.func].typeidx];
            }
            if(type != FuncType()){
                throw Exception::Exception("invalid explicit start type");
            }
            output.start.emplace(index);
            return;
        }
    }
    throw Exception::Exception("explicit start module not found");
}