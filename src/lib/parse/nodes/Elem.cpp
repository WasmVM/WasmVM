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
        elemid_map[node.id] = module.elems.size();
    }

    WasmElem& elem = module.elems.emplace_back();

    // mode
    if(node.offset){
        if(node.tableidx){
            elem.mode.tableidx = std::visit(Parse::Index::Visitor(table_indices.id_map), node.tableidx.value());
        }else{
            elem.mode.tableidx = 0;
        }
        std::vector<WasmVM::ConstInstr> instrs;
        std::visit(InstrVisitor::ConstSema(*this, instrs), node.offset.value());
        elem.mode.offset = instrs.front();
        elem.mode.type = WasmElem::ElemMode::Mode::active;
    }else{
        elem.mode.type = node.declarative ? WasmElem::ElemMode::Mode::declarative : WasmElem::ElemMode::Mode::passive;
    }
    // reftype
    elem.type = node.reftype;
    // itemlist
    for(auto item : node.elemlist){
        std::visit(overloaded {
            [&](Parse::Instr::ConstInstr& instr){
                std::vector<WasmVM::ConstInstr> instrs;
                std::visit(InstrVisitor::ConstSema(*this, instrs), instr);
                elem.elemlist.emplace_back(instrs.front());
            },
            [&](Parse::Index& index){
                elem.elemlist.emplace_back(Instr::Ref_func(std::visit(Parse::Index::Visitor(func_indices.id_map), index)));
            }
        }, item);
    }
}

std::optional<Parse::Elem> Parse::Elem::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using active_rule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Index, Token::ParenR>>,
        Token::ParenL, Parse::Optional<Token::Keyword<"offset">>, Syntax::ConstInstr, Token::ParenR
    >;
    using itemfull_rule = Parse::Rule<Syntax::RefType, Parse::Repeat<Parse::Rule<Token::ParenL, Parse::Optional<Token::Keyword<"item">>, Syntax::ConstInstr, Token::ParenR>>>;
    using itemabbr_rule = Parse::Rule<Parse::Optional<Token::Keyword<"func">>, Parse::Repeat<Parse::Index>>;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"elem">, Parse::Optional<Token::Id>,
        Parse::Optional<Parse::OneOf<active_rule, Token::Keyword<"declare">>>,
        Parse::OneOf<itemfull_rule, itemabbr_rule>,
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
                [&](active_rule& act){
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
        // elemlist
        std::visit(overloaded {
            [&](itemfull_rule& items){
                if(std::holds_alternative<Token::Keyword<"funcref">>(std::get<0>(items))){
                    elem.reftype = RefType::funcref;
                }else{
                    elem.reftype = RefType::externref;
                }
                for(auto item : std::get<1>(items)){
                    std::visit(overloaded {
                        [&](auto& instr){
                            elem.elemlist.emplace_back(instr);
                        }
                    }, std::get<2>(item));
                }
            },
            [&](itemabbr_rule& items){
                elem.reftype = RefType::funcref;
                for(Parse::Index& index : std::get<1>(items)){
                    elem.elemlist.emplace_back(index);
                }
            },
        }, std::get<4>(rule));
        begin = it;
        return elem;
    }
    return std::nullopt;
}