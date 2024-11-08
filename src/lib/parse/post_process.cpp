// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"

#include <unordered_map>
#include <queue>

using namespace WasmVM;

std::unordered_map<index_t, index_t> Visitor::reorder_map(IndexSpace& space){
    std::unordered_map<index_t, index_t> result;
    std::queue<index_t> indices;
    for(index_t i = 0; i < space.records.size(); ++i){
        if(space.records[i] == IndexSpace::Type::Import){
            result[i] = result.size();
        }else{
            indices.push(i);
        }
    }
    while(!indices.empty()){
        result[indices.front()] = result.size();
        indices.pop();
    }
    return result;
}

void Visitor::post_process(){
    std::unordered_map<index_t, index_t> typemap;    
    {
        // type
        std::unordered_map<FuncType, index_t> funcmap;
        std::vector<FuncType> functypes;
        for(index_t i = 0; i < module.types.size(); ++i){
            if(funcmap.contains(module.types[i])){
                typemap[i] = funcmap[module.types[i]];
            }else{
                funcmap[module.types[i]] = functypes.size();
                typemap[i] = functypes.size();
                functypes.emplace_back(module.types[i]);
            }
        }
        module.types = functypes;
    }
    std::unordered_map<index_t, index_t> funcmap = reorder_map(func_map);
    std::unordered_map<index_t, index_t> tablemap = reorder_map(table_map);
    std::unordered_map<index_t, index_t> memorymap = reorder_map(mem_map);
    std::unordered_map<index_t, index_t> globalmap = reorder_map(global_map);

    // funcs
    for(WasmFunc& func : module.funcs){
        if(typemap.contains(func.typeidx)){
            func.typeidx = typemap[func.typeidx];
        }
        for(WasmInstr& instr : func.body){
            std::visit(overloaded {
                [&](Instr::Block& instr){
                    if(instr.type && typemap.contains(instr.type.value())){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::Loop& instr){
                    if(instr.type && typemap.contains(instr.type.value())){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::If& instr){
                    if(instr.type && typemap.contains(instr.type.value())){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::Call& instr){
                    // if(funcmap.contains(instr.index)){
                    //     instr.index = funcmap[instr.index];
                    // }
                },
                [&](Instr::Call_indirect& instr){
                    // if(tablemap.contains(instr.tableidx)){
                    //     instr.tableidx = tablemap[instr.tableidx];
                    // }
                    if(typemap.contains(instr.typeidx)){
                        instr.typeidx = typemap[instr.typeidx];
                    }
                },
                [&](Instr::Ref_func& instr){
                    if(funcmap.contains(instr.index)){
                        instr.index = funcmap[instr.index];
                    }
                },
                [&](Instr::Global_get& instr){
                    if(globalmap.contains(instr.index)){
                        instr.index = globalmap[instr.index];
                    }
                },
                [&](Instr::Global_set& instr){
                    if(globalmap.contains(instr.index)){
                        instr.index = globalmap[instr.index];
                    }
                },
                [&](Instr::Table_get& instr){
                    if(tablemap.contains(instr.index)){
                        instr.index = tablemap[instr.index];
                    }
                },
                [&](Instr::Table_set& instr){
                    if(tablemap.contains(instr.index)){
                        instr.index = tablemap[instr.index];
                    }
                },
                [&](Instr::Table_size& instr){
                    if(tablemap.contains(instr.index)){
                        instr.index = tablemap[instr.index];
                    }
                },
                [&](Instr::Table_grow& instr){
                    if(tablemap.contains(instr.index)){
                        instr.index = tablemap[instr.index];
                    }
                },
                [&](Instr::Table_fill& instr){
                    if(tablemap.contains(instr.index)){
                        instr.index = tablemap[instr.index];
                    }
                },
                [&](Instr::Table_copy& instr){
                    if(tablemap.contains(instr.dstidx)){
                        instr.dstidx = tablemap[instr.dstidx];
                    }
                    if(tablemap.contains(instr.srcidx)){
                        instr.srcidx = tablemap[instr.srcidx];
                    }
                },
                [&](Instr::Table_init& instr){
                    if(tablemap.contains(instr.tableidx)){
                        instr.tableidx = tablemap[instr.tableidx];
                    }
                },
                [&](Instr::I32_load& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::F32_load& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::F64_load& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_load8_s& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_load8_u& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_load16_s& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_load16_u& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load8_s& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load8_u& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load16_s& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load16_u& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load32_s& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_load32_u& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_store& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_store& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::F32_store& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::F64_store& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_store8& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I32_store16& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_store8& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_store16& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::I64_store32& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::Memory_size& instr){
                    if(memorymap.contains(instr.index)){
                        instr.index = memorymap[instr.index];
                    }
                },
                [&](Instr::Memory_grow& instr){
                    if(memorymap.contains(instr.index)){
                        instr.index = memorymap[instr.index];
                    }
                },
                [&](Instr::Memory_fill& instr){
                    if(memorymap.contains(instr.index)){
                        instr.index = memorymap[instr.index];
                    }
                },
                [&](Instr::Memory_init& instr){
                    if(memorymap.contains(instr.memidx)){
                        instr.memidx = memorymap[instr.memidx];
                    }
                },
                [&](Instr::Memory_copy& instr){
                    if(memorymap.contains(instr.dstidx)){
                        instr.dstidx = memorymap[instr.dstidx];
                    }
                    if(memorymap.contains(instr.srcidx)){
                        instr.srcidx = memorymap[instr.srcidx];
                    }
                },
                [](auto&){}
            }, instr);
        }
    }
    // elems
    for(WasmElem& elem : module.elems){
        if(elem.mode.tableidx && tablemap.contains(elem.mode.tableidx.value())){
            elem.mode.tableidx = tablemap[elem.mode.tableidx.value()];
        }
        for(ConstInstr& item : elem.elemlist){
                std::visit(overloaded {
                    [&](Instr::Global_get& instr){
                        if(globalmap.contains(instr.index)){
                            instr.index = globalmap[instr.index];
                        }
                    },
                    [&](Instr::Ref_func& instr){
                        if(funcmap.contains(instr.index)){
                            instr.index = funcmap[instr.index];
                        }
                    },
                    [&](Instr::I32_const& instr){
                        if(funcmap.contains(instr.value)){
                            instr.value = funcmap[instr.value];
                        }
                    },
                    [&](Instr::I64_const& instr){
                        if(funcmap.contains(instr.value)){
                            instr.value = funcmap[instr.value];
                        }
                    },
                    [&](Instr::F32_const& instr){
                        if(funcmap.contains(instr.value)){
                            instr.value = (instr.value - (index_t)instr.value) + funcmap[instr.value];
                        }
                    },
                    [&](Instr::F64_const& instr){
                        if(funcmap.contains(instr.value)){
                            instr.value = (instr.value - (index_t)instr.value) + funcmap[instr.value];
                        }
                    },
                    [](auto&){}
                }, item);
            }
        if(elem.mode.offset){
            std::visit(overloaded {
                [&](Instr::Global_get& instr){
                    if(globalmap.contains(instr.index)){
                        instr.index = globalmap[instr.index];
                    }
                },
                [](auto&){}
            }, elem.mode.offset.value());
        }
    }
    // datas
    for(WasmData& data : module.datas){
        if(data.mode.memidx && memorymap.contains(data.mode.memidx.value())){
            data.mode.memidx = memorymap[data.mode.memidx.value()];
        }
        if(data.mode.offset){
            std::visit(overloaded {
                [&](Instr::Global_get& instr){
                    if(globalmap.contains(instr.index)){
                        instr.index = globalmap[instr.index];
                    }
                },
                [](auto&){}
            }, data.mode.offset.value());
        }
    }
    // start
    if(module.start && funcmap.contains(module.start.value())){
        module.start = funcmap[module.start.value()];
    }
    // exports
    for(WasmExport& export_ : module.exports){
        switch(export_.desc){
            case WasmExport::DescType::func :
                if(funcmap.contains(export_.index)){
                    export_.index = funcmap[export_.index];
                }
            break;
            case WasmExport::DescType::table :
                if(tablemap.contains(export_.index)){
                    export_.index = tablemap[export_.index];
                }
            break;
            case WasmExport::DescType::mem :
                if(memorymap.contains(export_.index)){
                    export_.index = memorymap[export_.index];
                }
            break;
            case WasmExport::DescType::global :
                if(globalmap.contains(export_.index)){
                    export_.index = globalmap[export_.index];
                }
            break;
        }
    }
    // imports
    for(WasmImport& import : module.imports){
        if(std::holds_alternative<index_t>(import.desc) && typemap.contains(std::get<index_t>(import.desc))){
            import.desc = typemap[std::get<index_t>(import.desc)];
        }
    }
};