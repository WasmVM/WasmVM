// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Data& node){
    // id
    if(!node.id.empty()){
        if(dataid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : data ") + node.id);
        }
        dataid_map[node.id] = module.datas.size();
    }

    WasmData& data = module.datas.emplace_back();

    // mode
    if(node.offset){
        if(node.memidx){
            data.mode.memidx = std::visit(Parse::Index::Visitor(mem_indices.id_map), node.memidx.value());
        }else{
            data.mode.memidx = 0;
        }
        std::vector<WasmVM::ConstInstr> instrs;
        std::visit(InstrVisitor::ConstSema(*this, instrs), node.offset.value());
        data.mode.offset = instrs.front();
        data.mode.type = WasmData::DataMode::Mode::active;
    }else{
        data.mode.type = WasmData::DataMode::Mode::passive;
    }
    // init
    for(Token::String str : node.inits){
        std::transform(str.value.begin() + 1, str.value.end() - 1, std::back_inserter(data.init), [](char c){
            return std::byte(c);
        });
    }
}

std::optional<Parse::Data> Parse::Data::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using active_rule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"memory">, Parse::Index, Token::ParenR>>,
        Token::ParenL, Parse::Optional<Token::Keyword<"offset">>, Syntax::ConstInstr, Token::ParenR
    >;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"data">, Parse::Optional<Token::Id>,
        Parse::Optional<active_rule>, Parse::Repeat<Token::String>,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Data data;
        auto rule = syntax.value();
        // location
        data.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        data.id = id ? id->value : "";
        // mode
        auto mode = std::get<3>(rule);
        if(mode){
            auto memidx_rule = std::get<0>(mode.value());
            if(memidx_rule){
                data.memidx = std::get<2>(memidx_rule.value());
            }
            std::vector<Parse::Instr::ConstInstr> instr;
            InstrVisitor::ConstSyntax visitor(instr);
            visitor(std::get<3>(mode.value()));
            data.offset = instr.front();
        }
        // datastr
        data.inits = std::get<4>(rule);
        begin = it;
        return data;
    }
    return std::nullopt;
}