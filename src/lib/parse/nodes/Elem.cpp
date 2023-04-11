// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Elem& node){
    // id
    if(!node.id.empty()){
        if(elemid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : elem ") + node.id);
        }
        elemid_map[node.id] = elemid_map.size();
    }

    WasmElem& elem = module.elems.emplace_back();

    // mode
    if(node.offset){
        if(node.tableidx){
            
        }
    }else{
        elem.mode.type = node.declarative ? WasmElem::ElemMode::Mode::declarative : WasmElem::ElemMode::Mode::passive;
    }
}

std::optional<Parse::Elem> Parse::Elem::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using activerule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Index, Token::ParenR>>,
        Token::ParenL, Parse::Optional<Token::Keyword<"offset">>, Syntax::ConstInstr, Token::ParenR,
        Token::ParenR
    >;
    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"elem">, Parse::Optional<Token::Id>,
        Parse::Optional<Parse::OneOf<activerule, Token::Keyword<"declare">>>,
        Parse::OneOf<
            Parse::Rule<Syntax::RefType, Parse::Repeat<Parse::Rule<Token::ParenL, Parse::Optional<Token::Keyword<"item">>, Syntax::ConstInstr, Token::ParenR>>>,
            Parse::Rule<Token::Keyword<"func">, Parse::Repeat<Parse::Index>>
        >,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Elem elem;
        auto rule = syntax.value();
        // location
        elem.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        elem.id = id ? id->value : "";
        // mode
        auto mode = std::get<3>(rule);
        if(mode){
            std::visit(overloaded {
                [&](activerule& act){
                    auto tableidx = std::get<0>(act);
                    if(tableidx){
                        elem.tableidx = std::get<2>(tableidx.value());
                    }
                    std::vector<Instr::ConstInstr> offset;
                    InstrVisitor::ConstSyntax visitor(offset);
                    visitor(std::get<3>(act));
                    elem.offset = offset.front();
                },
                [&](Token::Keyword<"declare">&){
                    elem.declarative = true;
                },
            }, mode.value());
        }

        begin = it;
        return elem;
    }
    return std::nullopt;
}