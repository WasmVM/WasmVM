// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Export& node){
    WasmExport& export_ = module.exports.emplace_back();
    export_.name = node.name;
    export_.desc = node.type;
    switch(node.type){
        case WasmExport::DescType::func :
            export_.index = std::visit(Parse::Index::Visitor(func_indices.id_map), node.index);
        break;
        case WasmExport::DescType::table :
            export_.index = std::visit(Parse::Index::Visitor(table_indices.id_map), node.index);
        break;
        case WasmExport::DescType::mem :
            export_.index = std::visit(Parse::Index::Visitor(mem_indices.id_map), node.index);
        break;
        case WasmExport::DescType::global :
            export_.index = std::visit(Parse::Index::Visitor(global_indices.id_map), node.index);
        break;
    }
}

std::optional<Parse::Export> Parse::Export::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"export">, Token::String,
        Token::ParenL, Parse::OneOf<Token::Keyword<"func">, Token::Keyword<"table">, Token::Keyword<"memory">, Token::Keyword<"global">>,
        Parse::Index, Token::ParenR,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Export export_(std::get<5>(rule));
        // location
        export_.location = std::get<0>(rule).location;
        // name
        export_.name = std::get<2>(rule).value.substr(1, std::get<2>(rule).value.size() - 2);
        // desc
        std::visit(overloaded {
            [&](Token::Keyword<"func">&){
                export_.type = WasmExport::DescType::func;
            },
            [&](Token::Keyword<"table">&){
                export_.type = WasmExport::DescType::table;
            },
            [&](Token::Keyword<"memory">&){
                export_.type = WasmExport::DescType::mem;
            },
            [&](Token::Keyword<"global">&){
                export_.type = WasmExport::DescType::global;
            }
        }, std::get<4>(rule));
        begin = it;
        return export_;
    }
    return std::nullopt;
}