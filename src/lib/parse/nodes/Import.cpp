// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../syntax.hpp"
#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Import& node){
    WasmImport& import = module.imports.emplace_back();
    std::visit(overloaded {
        // Func
        [&](Parse::TypeUse typeuse){
            import.desc.emplace<index_t>(Parse::Type::index(typeuse, module, typeid_map, paramid_maps));
            // id
            if(!node.id.empty()){
                if(func_indices.id_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import func ") + node.id);
                }
                func_indices.id_map[node.id] = func_indices.records.size();
            }
            func_indices.records.emplace_back(IndexSpace::Type::Import);
        },
        // Table
        [&](Parse::TableType tabletype){
            import.desc.emplace<WasmVM::TableType>(tabletype);
            if(!node.id.empty()){
                if(table_indices.id_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import table ") + node.id);
                }
                table_indices.id_map[node.id] = table_indices.records.size();
            }
            table_indices.records.emplace_back(IndexSpace::Type::Import);
        },
        // Memory
        [&](Parse::MemType memtype){
            import.desc.emplace<WasmVM::MemType>(memtype);
            if(!node.id.empty()){
                if(mem_indices.id_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import memory ") + node.id);
                }
                mem_indices.id_map[node.id] = mem_indices.records.size();
            }
            mem_indices.records.emplace_back(IndexSpace::Type::Import);
        },
        // Global
        [&](Parse::GlobalType globaltype){
            import.desc.emplace<WasmVM::GlobalType>(globaltype);
            if(!node.id.empty()){
                if(global_indices.id_map.contains(node.id)){
                    throw Exception::duplicated_identifier(node.location, std::string(" : import global ") + node.id);
                }
                global_indices.id_map[node.id] = global_indices.records.size();
            }
            global_indices.records.emplace_back(IndexSpace::Type::Import);
        }
    }, node.desc);
    import.module = node.module;
    import.name = node.name;
}

std::optional<Parse::Import> Parse::Import::get(TokenIter& begin, TokenHolder& holder){
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
    >::get(it, holder);

    if(syntax){
        Parse::Import import;
        auto rule = syntax.value();
        import.module = std::get<2>(rule).value.substr(1, std::get<2>(rule).value.size() - 2);
        import.name = std::get<3>(rule).value.substr(1, std::get<3>(rule).value.size() - 2);
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
    import.desc.emplace<Parse::TypeUse>(std::get<3>(desc));
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