// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Table& node){
    index_t tableidx = table_indices.records.size();
    // export
    for(std::string name : node.exports){
        WasmExport& export_ = module.exports.emplace_back();
        export_.name = name;
        export_.desc = WasmExport::DescType::table;
        export_.index = tableidx;
    }
    // import
    if(!node.import.first.empty() || !node.import.second.empty()){
        WasmImport& import = module.imports.emplace_back();
        if(!node.id.empty()){
            if(table_indices.id_map.contains(node.id)){
                throw Exception::duplicated_identifier(node.location, std::string(" : table ") + node.id);
            }
            table_indices.id_map[node.id] = tableidx;
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
        table_indices.id_map[node.id] = tableidx;
    }
    table_indices.records.emplace_back(IndexSpace::Type::Normal);
    // table
    module.tables.emplace_back(node.tabletype);
    // elem
    if(node.elemlist.size() > 0){
        WasmElem& elem = module.elems.emplace_back();
        elem.type = node.tabletype.reftype;
        elem.mode.type = WasmElem::ElemMode::Mode::active;
        elem.mode.tableidx = tableidx;
        elem.mode.offset.emplace(WasmVM::Instr::I32_const(0));
        for(Parse::Instr::ConstInstr& item : node.elemlist){
            std::visit(InstrVisitor::ConstSema(*this, elem.elemlist), item);
        }
    }
}

std::optional<Parse::Table> Parse::Table::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using tablerule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, Token::ParenR>>,
        Parse::TableType
    >;
    using itemrule = Parse::Rule<Token::ParenL, Parse::Optional<Token::Keyword<"item">>, Syntax::ConstInstr, Token::ParenR>;
    using elemrule = Parse::Rule<
        Syntax::RefType,
        Token::ParenL, Token::Keyword<"elem">,
        Parse::Repeat<Parse::OneOf<Parse::Index, itemrule>>,
        Token::ParenR
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
            table.exports.emplace_back(std::get<2>(node).value.substr(1, std::get<2>(node).value.size() - 2));
        }
        // import/elem
        std::visit(overloaded {
            // Import
            [&](tablerule& node){
                auto importnode = std::get<0>(node);
                if(importnode){
                    table.import = std::pair<std::string, std::string>(std::get<2>(importnode.value()).value.substr(1, std::get<2>(importnode.value()).value.size() - 2), std::get<3>(importnode.value()).value.substr(1, std::get<3>(importnode.value()).value.size() - 2));
                }
                table.tabletype = std::get<1>(node);
            },
            // elem
            [&](elemrule& node){
                // reftype
                std::visit(overloaded {
                    [&](Token::Keyword<"funcref">){
                        table.tabletype.reftype = WasmVM::RefType::funcref;
                    },
                    [&](Token::Keyword<"externref">){
                        table.tabletype.reftype = WasmVM::RefType::externref;
                    }
                }, std::get<0>(node));
                // items
                auto items = std::get<3>(node);
                table.tabletype.limits.min = items.size();
                table.tabletype.limits.max = items.size();
                for(auto item : items){
                    std::visit(overloaded {
                        [&](Parse::Index& index){
                            table.elemlist.emplace_back<Instr::Ref_func>(index);
                        },
                        [&](itemrule& instr){
                            InstrVisitor::ConstSyntax(table.elemlist)(std::get<2>(instr));
                        },
                    }, item);
                }
            }
        }, std::get<4>(rule));

        begin = it;
        return table;
    }
    return std::nullopt;
}