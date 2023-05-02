// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "../syntax.hpp"
#include "visitor.hpp"

#include <unordered_map>
#include <queue>
#include <iostream>

#include "../../dump/dump.hpp"

using namespace WasmVM;

static std::unordered_map<index_t, index_t> reorder_map(ModuleVisitor::IndexSpace& space){
    std::unordered_map<index_t, index_t> result;
    std::queue<index_t> indices;
    for(index_t i = 0; i < space.records.size(); ++i){
        if(space.records[i] == ModuleVisitor::IndexSpace::Type::Import){
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

static void post_process(ModuleVisitor& visitor){
    std::unordered_map<index_t, index_t> typemap;    
    {
        // type
        std::unordered_map<FuncType, index_t> funcmap;
        std::vector<FuncType> functypes;
        for(index_t i = 0; i < visitor.module.types.size(); ++i){
            if(funcmap.contains(visitor.module.types[i])){
                typemap[i] = funcmap[visitor.module.types[i]];
            }else{
                funcmap[visitor.module.types[i]] = functypes.size();
                typemap[i] = functypes.size();
                functypes.emplace_back(visitor.module.types[i]);
            }
        }
        visitor.module.types = functypes;
    }
    std::unordered_map<index_t, index_t> funcmap = reorder_map(visitor.func_indices);
    std::unordered_map<index_t, index_t> tablemap = reorder_map(visitor.table_indices);
    std::unordered_map<index_t, index_t> memorymap = reorder_map(visitor.mem_indices);
    std::unordered_map<index_t, index_t> globalmap = reorder_map(visitor.global_indices);

    // funcs
    for(WasmFunc& func : visitor.module.funcs){
        func.typeidx = typemap[func.typeidx];
        for(WasmInstr& instr : func.body){
            std::visit(overloaded {
                [&](Instr::Block& instr){
                    if(instr.type){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::Loop& instr){
                    if(instr.type){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::If& instr){
                    if(instr.type){
                        instr.type = typemap[instr.type.value()];
                    }
                },
                [&](Instr::Call& instr){
                    instr.index = funcmap[instr.index];
                },
                [&](Instr::Call_indirect& instr){
                    instr.tableidx = tablemap[instr.tableidx];
                    instr.typeidx = typemap[instr.typeidx];
                },
                [&](Instr::Ref_func& instr){
                    instr.index = funcmap[instr.index];
                },
                [&](Instr::Global_get& instr){
                    instr.index = globalmap[instr.index];
                },
                [&](Instr::Global_set& instr){
                    instr.index = globalmap[instr.index];
                },
                [&](Instr::Table_get& instr){
                    instr.index = tablemap[instr.index];
                },
                [&](Instr::Table_set& instr){
                    instr.index = tablemap[instr.index];
                },
                [&](Instr::Table_size& instr){
                    instr.index = tablemap[instr.index];
                },
                [&](Instr::Table_grow& instr){
                    instr.index = tablemap[instr.index];
                },
                [&](Instr::Table_fill& instr){
                    instr.index = tablemap[instr.index];
                },
                [&](Instr::Table_copy& instr){
                    instr.dstidx = tablemap[instr.dstidx];
                    instr.srcidx = tablemap[instr.srcidx];
                },
                [&](Instr::Table_init& instr){
                    instr.tableidx = tablemap[instr.tableidx];
                },
                [&](Instr::I32_load& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::F32_load& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::F64_load& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_load8_s& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_load8_u& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_load16_s& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_load16_u& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load8_s& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load8_u& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load16_s& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load16_u& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load32_s& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_load32_u& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_store& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_store& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::F32_store& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::F64_store& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_store8& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I32_store16& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_store8& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_store16& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::I64_store32& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::Memory_size& instr){
                    instr.index = memorymap[instr.index];
                },
                [&](Instr::Memory_grow& instr){
                    instr.index = memorymap[instr.index];
                },
                [&](Instr::Memory_fill& instr){
                    instr.index = memorymap[instr.index];
                },
                [&](Instr::Memory_init& instr){
                    instr.memidx = memorymap[instr.memidx];
                },
                [&](Instr::Memory_copy& instr){
                    instr.dstidx = memorymap[instr.dstidx];
                    instr.srcidx = memorymap[instr.srcidx];
                },
                [](auto&){}
            }, instr);
        }
        // TODO: func body
    }
    // elems
    for(WasmElem& elem : visitor.module.elems){
        if(elem.mode.tableidx){
            elem.mode.tableidx = tablemap[elem.mode.tableidx.value()];
        }
        if(elem.type == RefType::funcref){
            for(ConstInstr& item : elem.elemlist){
                std::visit(overloaded {
                    [&](Instr::Ref_func& instr){
                        instr.index = funcmap[instr.index];
                    },
                    [&](Instr::I32_const& instr){
                        instr.value = funcmap[instr.value];
                    },
                    [&](Instr::I64_const& instr){
                        instr.value = funcmap[instr.value];
                    },
                    [&](Instr::F32_const& instr){
                        instr.value = (instr.value - (index_t)instr.value) + funcmap[instr.value];
                    },
                    [&](Instr::F64_const& instr){
                        instr.value = (instr.value - (index_t)instr.value) + funcmap[instr.value];
                    },
                    [](auto&){}
                }, item);
            }
        }
    }
    // datas
    for(WasmData& data : visitor.module.datas){
        if(data.mode.memidx){
            data.mode.memidx = memorymap[data.mode.memidx.value()];
        }
    }
    // start
    if(visitor.module.start){
        visitor.module.start = funcmap[visitor.module.start.value()];
    }
    // exports
    for(WasmExport& export_ : visitor.module.exports){
        switch(export_.desc){
            case WasmExport::DescType::func :
                export_.index = funcmap[export_.index];
            break;
            case WasmExport::DescType::table :
                export_.index = tablemap[export_.index];
            break;
            case WasmExport::DescType::mem :
                export_.index = memorymap[export_.index];
            break;
            case WasmExport::DescType::global :
                export_.index = globalmap[export_.index];
            break;
        }
    }
    // imports
    for(WasmImport& import : visitor.module.imports){
        if(std::holds_alternative<index_t>(import.desc)){
            import.desc = typemap[std::get<index_t>(import.desc)];
        }
    }
};

WasmModule WasmVM::module_parse(std::istream& stream){
    std::list<TokenType> tokens = tokenize(stream);

    using modulefields = Parse::Repeat<Parse::OneOf<
        Parse::Type,
        Parse::Import,
        Parse::Func,
        Parse::Table,
        Parse::Memory,
        Parse::Global,
        Parse::Export,
        Parse::Start,
        Parse::Elem,
        Parse::Data,
        Parse::Rule<Token::ParenL, Token::ParenR>
    >>;

    using modulerule = Parse::Rule<
        Token::ParenL, Token::Keyword<"module">, Parse::Optional<Token::Id>, modulefields, Token::ParenR
    >;

    WasmModule wasm_module;
    TokenIter it = tokens.begin();
    TokenHolder holder(tokens.begin(), tokens.end());

    auto syntax = Parse::OneOf<modulerule, modulefields>::get(it, holder);

    if(syntax && !holder.has_next(it)){
        auto sections = std::visit(overloaded {
            [&](modulerule& rule){
                return std::get<3>(rule);
            },
            [&](modulefields& fields){
                return fields;
            }
        }, syntax.value());

        // sections
        ModuleVisitor visitor(wasm_module);
        for(auto section : sections){
            std::visit(visitor, section);
        }

        post_process(visitor);

        return wasm_module;
    }
    if(holder.has_next(it)){
        throw holder.error();
    }else{
        throw Exception::unexpected_eof();
    }
}