// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

#include <exception.hpp>

using namespace WasmVM;

void InstrVisitor::Sema::operator()(Parse::Instr::Call& node){
    index_t index = std::visit(Parse::Index::Visitor(module.funcid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Call>(index);
}
void InstrVisitor::Sema::operator()(Parse::Instr::Block& node){
    WasmVM::Instr::Block instr(std::nullopt);
    if(node.blocktype){
        instr = std::visit(overloaded {
            [&](ValueType& type) {
                return WasmVM::Instr::Block(std::optional<ValueType>(type));
            },
            [&](Parse::TypeUse& typeuse) {
                Parse::Type type(typeuse);
                if(!type.func.id_map.empty()){
                    Exception::Warning("param ids in block are ignored");
                }
                return WasmVM::Instr::Block(type.index(module.module, module.typeid_map, module.paramid_maps));
            },
        }, node.blocktype.value());
    }
    func.body.emplace_back(instr);
    for(Parse::Instr::Instrction instrnode : node.instrs){
        std::visit(InstrVisitor::Sema(module, func), instrnode);
    }
    func.body.emplace_back(WasmVM::Instr::End());
}
void InstrVisitor::Sema::operator()(Parse::Instr::Loop& node){
    WasmVM::Instr::Loop instr(std::nullopt);
    if(node.blocktype){
        instr = std::visit(overloaded {
            [&](ValueType& type) {
                return WasmVM::Instr::Loop(std::optional<ValueType>(type));
            },
            [&](Parse::TypeUse& typeuse) {
                Parse::Type type(typeuse);
                if(!type.func.id_map.empty()){
                    Exception::Warning("param ids in loop are ignored");
                }
                return WasmVM::Instr::Loop(type.index(module.module, module.typeid_map, module.paramid_maps));
            },
        }, node.blocktype.value());
    }
    func.body.emplace_back(instr);
    for(Parse::Instr::Instrction instrnode : node.instrs){
        std::visit(InstrVisitor::Sema(module, func), instrnode);
    }
    func.body.emplace_back(WasmVM::Instr::End());
}
void InstrVisitor::Syntax::operator()(WasmVM::Syntax::PlainInstr& plain){
    std::visit(overloaded {
        [&](auto&& instr){
            body.emplace_back(instr);
        }
    }, plain);
}
void InstrVisitor::Syntax::operator()(Parse::Instr::Block& block){
    body.emplace_back(block);
}
void InstrVisitor::Syntax::operator()(Parse::Instr::Loop& block){
    body.emplace_back(block);
}

std::optional<Parse::Instr::Block> Parse::Instr::Block::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"block">, Parse::Optional<Token::Id>,
        Parse::Optional<Parse::OneOf<Parse::ValueType, Parse::TypeUse>>,
        Parse::Repeat<Syntax::Instr>,
        Token::Keyword<"end">, Parse::Optional<Token::Id>
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Block block;
        block.location = std::get<0>(rule).location;
        // id
        auto id = std::get<1>(rule);
        block.id = id ? id->value : "";
        // blocktype
        auto blocktype = std::get<2>(rule);
        if(blocktype){
            std::variant<WasmVM::ValueType, Parse::TypeUse> type;
            std::visit(overloaded {
                [&](Parse::ValueType& value){
                    type.emplace<WasmVM::ValueType>(value);
                },
                [&](Parse::TypeUse& value){
                    type.emplace<Parse::TypeUse>(value);
                }
            }, blocktype.value());
            block.blocktype.emplace(type);
        }
        // instrs
        for(auto instr : std::get<3>(rule)){
            std::visit(InstrVisitor::Syntax(block.instrs), instr);
        }
        // trailing id
        auto trailing = std::get<5>(rule);
        if(trailing && !id){
            throw Exception::block_id_mismatch(block.location, std::string(" : leading identifier is not present"));
        }else if(id && trailing && (trailing->value != id->value)){
            throw Exception::block_id_mismatch(trailing->location, std::string(" : leading & trailing id should be the same"));
        }
        begin = it;
        return block;
    }
    return std::nullopt;
}

std::optional<Parse::Instr::Loop> Parse::Instr::Loop::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"loop">, Parse::Optional<Token::Id>,
        Parse::Optional<Parse::OneOf<Parse::ValueType, Parse::TypeUse>>,
        Parse::Repeat<Syntax::Instr>,
        Token::Keyword<"end">, Parse::Optional<Token::Id>
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Loop loop;
        loop.location = std::get<0>(rule).location;
        // id
        auto id = std::get<1>(rule);
        loop.id = id ? id->value : "";
        // blocktype
        auto blocktype = std::get<2>(rule);
        if(blocktype){
            std::variant<WasmVM::ValueType, Parse::TypeUse> type;
            std::visit(overloaded {
                [&](Parse::ValueType& value){
                    type.emplace<WasmVM::ValueType>(value);
                },
                [&](Parse::TypeUse& value){
                    type.emplace<Parse::TypeUse>(value);
                }
            }, blocktype.value());
            loop.blocktype.emplace(type);
        }
        // instrs
        for(auto instr : std::get<3>(rule)){
            std::visit(InstrVisitor::Syntax(loop.instrs), instr);
        }
        // trailing id
        auto trailing = std::get<5>(rule);
        if(trailing && !id){
            throw Exception::block_id_mismatch(loop.location, std::string(" : leading identifier is not present"));
        }else if(id && trailing && (trailing->value != id->value)){
            throw Exception::block_id_mismatch(trailing->location, std::string(" : leading & trailing id should be the same"));
        }
        begin = it;
        return loop;
    }
    return std::nullopt;
}