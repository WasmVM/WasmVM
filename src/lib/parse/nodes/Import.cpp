// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Import& node){
    WasmImport& import = module.imports.emplace_back();
    std::visit(overloaded {
        // Type
        [&](Parse::Type type){
            index_t idx = std::visit(overloaded {
                [&](std::string id){
                    if(id.empty()){
                        return index_npos;
                    }else if(typeid_map.contains(id)){
                        return typeid_map[id];
                    }else{
                        throw Exception::unknown_identifier(node.location, std::string(" import type '") + id + "' not found");
                    }
                },
                [&](index_t id){
                    return id;
                }
            }, type.id);

            FuncType functype;
            std::map<std::string, index_t> paramid_map;
            if(idx != index_npos){
                if(idx >= module.types.size()){
                    throw Exception::index_out_of_range(node.location, " : type not found");
                }
                if(type.func.params.empty() && type.func.results.empty()){
                    import.desc.emplace<index_t>(idx);
                    paramid_maps.emplace_back(paramid_maps[idx]);
                    if(!node.id.empty()){
                        if(funcid_map.contains(node.id)){
                            throw Exception::duplicated_identifier(node.location, std::string(" : import func ") + node.id);
                        }
                        funcid_map[node.id] = funcidx;
                    }
                    funcidx += 1;
                    return;
                }
                functype = module.types[idx];
                paramid_map = paramid_maps[idx];
            }
            size_t param_count = functype.params.size();
            for(auto id_pair : type.func.id_map){
                if(paramid_map.contains(id_pair.first)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import func param ") + id_pair.first);
                }
                paramid_map[id_pair.first] = id_pair.second + param_count;
            }
            functype.params.insert(functype.params.end(), type.func.params.begin(), type.func.params.end());
            functype.results.insert(functype.results.end(), type.func.results.begin(), type.func.results.end());
            import.desc.emplace<index_t>(module.types.size());
            paramid_maps.emplace_back(paramid_map);
            module.types.emplace_back(functype);
            // id
            if(!node.id.empty()){
                if(funcid_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import func ") + node.id);
                }
                funcid_map[node.id] = funcidx;
            }
            typeidx += 1;
            funcidx += 1;
        },
        // Table
        [&](Parse::TableType tabletype){
            import.desc.emplace<WasmVM::TableType>(tabletype);
            if(!node.id.empty()){
                if(tableid_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import table ") + node.id);
                }
                tableid_map[node.id] = tableidx;
            }
            tableidx += 1;
        },
        // Memory
        [&](Parse::MemType memtype){
            import.desc.emplace<WasmVM::MemType>(memtype);
            if(!node.id.empty()){
                if(memid_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import memory ") + node.id);
                }
                memid_map[node.id] = memidx;
            }
            memidx += 1;
        },
        // Global
        [&](Parse::GlobalType globaltype){
            import.desc.emplace<WasmVM::GlobalType>(globaltype);
            if(!node.id.empty()){
                if(globalid_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import global ") + node.id);
                }
                globalid_map[node.id] = globalidx;
            }
            globalidx += 1;
        }
    }, node.desc);
    import.module = node.module;
    import.name = node.name;
}

std::optional<Parse::Import> Parse::Import::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, 
        Parse::OneOf<
            Syntax::ImportDesc::Func,
            Syntax::ImportDesc::Table,
            Syntax::ImportDesc::Memory,
            Syntax::ImportDesc::Global
        >,
        Token::ParenR
    >::get(it, end);

    if(syntax){
        Parse::Import import;
        auto rule = syntax.value();
        import.module = std::get<2>(rule).value;
        import.name = std::get<3>(rule).value;
        std::visit(ImportVisitor(import), std::get<4>(rule));
        begin = it;
        return import;
    }

    return std::nullopt;
}

void ImportVisitor::operator()(Syntax::ImportDesc::Func& desc){
    auto id = std::get<2>(desc);
    import.id = id ? id->value : "";
    import.location = id->location;
    import.desc.emplace<Parse::Type>(std::get<3>(desc));
}

void ImportVisitor::operator()(Syntax::ImportDesc::Table& desc){
    auto id = std::get<2>(desc);
    import.id = id ? id->value : "";
    import.location = id->location;
    import.desc.emplace<Parse::TableType>(std::get<3>(desc));
}

void ImportVisitor::operator()(Syntax::ImportDesc::Memory& desc){
    auto id = std::get<2>(desc);
    import.id = id ? id->value : "";
    import.location = id->location;
    import.desc.emplace<Parse::MemType>(std::get<3>(desc));
}

void ImportVisitor::operator()(Syntax::ImportDesc::Global& desc){
    auto id = std::get<2>(desc);
    import.id = id ? id->value : "";
    import.location = id->location;
    import.desc.emplace<Parse::GlobalType>(std::get<3>(desc));
}