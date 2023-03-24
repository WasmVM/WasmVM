// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Table& node){
    // export
    for(std::string name : node.exports){
        module.exports.emplace_back(name, WasmExport::DescType::table, table_indices.records.size());
    }
    // import
    if(!node.import.first.empty() || !node.import.second.empty()){
        WasmImport& import = module.imports.emplace_back();
        if(!node.id.empty()){
            if(table_indices.id_map.contains(node.id)){
                throw Exception::duplicated_identifier(node.location, std::string(" : table ") + node.id);
            }
            table_indices.id_map[node.id] = table_indices.records.size();
        }
        import.module = node.import.first;
        import.name = node.import.second;
        import.desc.emplace<WasmVM::TableType>(node.tabletype);
        table_indices.records.emplace_back(IndexSpace::Type::Import);
        return;
    }
    // id
    if(!node.id.empty()){
        if(table_indices.id_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : table ") + node.id);
        }
        table_indices.id_map[node.id] = table_indices.records.size();
    }
    table_indices.records.emplace_back(IndexSpace::Type::Normal);
    // table
    module.tables.emplace_back(node.tabletype);
}

std::optional<Parse::Table> Parse::Table::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using tablerule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, Token::ParenR>>,
        Parse::TableType
    >;
    using elemrule = Parse::Rule<
        Parse::OneOf<Token::Keyword<"funcref">, Token::Keyword<"externref">>,
        Token::ParenL, Token::Keyword<"elem">, Parse::OneOf<Parse::Repeat<Syntax::ConstInstr>, Parse::Repeat<Parse::Index>>, Token::ParenR
    >;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"table">, Parse::Optional<Token::Id>,
        Parse::Repeat<Parse::Rule<Token::ParenL, Token::Keyword<"export">, Token::String, Token::ParenR>>,
        Parse::OneOf<tablerule, elemrule>,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Table table;
        auto rule = syntax.value();
        // location
        table.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        table.id = id ? id->value : "";
        // export
        for(auto node : std::get<3>(rule)){
            table.exports.emplace_back(std::get<2>(node).value);
        }
        // import/elem
        std::visit(overloaded {
            [&](tablerule& node){
                auto importnode = std::get<0>(node);
                if(importnode){
                    table.import = std::pair<std::string, std::string>(std::get<2>(importnode.value()).value, std::get<3>(importnode.value()).value);
                }
                table.tabletype = std::get<1>(node);
            },
            [&](elemrule& node){
                // TODO:
            }
        }, std::get<4>(rule));

        begin = it;
        return table;
    }
    return std::nullopt;
}