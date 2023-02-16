// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

#include <exception.hpp>

using namespace WasmVM;

struct BrIndexVisitor : public Parse::Index::Visitor {
    BrIndexVisitor(std::map<std::string, index_t>& idmap, bool check = true) : Parse::Index::Visitor(idmap, check) {}
    index_t operator()(Token::Number& token){
        return Parse::Index::Visitor::operator()(token);
    }
    index_t operator()(Token::Id& token){
        if(token.value.empty()){
            return index_npos;
        }else if(idmap.contains(token.value)){
            return idmap.size() - idmap[token.value] - 1;
        }else if(check){
            throw Exception::unknown_identifier(token.location, ": index '" + token.value + "' not found");
        }
        return index_npos;
    }
};

void InstrVisitor::Sema::operator()(Parse::Instr::Call& node){
    index_t index = std::visit(Parse::Index::Visitor(module.funcid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Call>(index);
}
void InstrVisitor::Sema::operator()(Parse::Instr::Block& node){
    WasmVM::Instr::Block instr(std::nullopt);
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
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
        std::visit(InstrVisitor::Sema(module, func, labelid_map), instrnode);
    }
    func.body.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}
void InstrVisitor::Sema::operator()(Parse::Instr::Loop& node){
    WasmVM::Instr::Loop instr(std::nullopt);
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
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
        std::visit(InstrVisitor::Sema(module, func, labelid_map), instrnode);
    }
    func.body.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}
void InstrVisitor::Sema::operator()(Parse::Instr::If& node){
    WasmVM::Instr::If instr(std::nullopt);
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        instr = std::visit(overloaded {
            [&](ValueType& type) {
                return WasmVM::Instr::If(std::optional<ValueType>(type));
            },
            [&](Parse::TypeUse& typeuse) {
                Parse::Type type(typeuse);
                if(!type.func.id_map.empty()){
                    Exception::Warning("param ids in if are ignored");
                }
                return WasmVM::Instr::If(type.index(module.module, module.typeid_map, module.paramid_maps));
            },
        }, node.blocktype.value());
    }
    func.body.emplace_back(instr);
    for(Parse::Instr::Instrction instrnode : node.instrs1){
        std::visit(InstrVisitor::Sema(module, func, labelid_map), instrnode);
    }
    if(!node.instrs2.empty()){
        func.body.emplace_back(WasmVM::Instr::Else());
        for(Parse::Instr::Instrction instrnode : node.instrs2){
            std::visit(InstrVisitor::Sema(module, func, labelid_map), instrnode);
        }
    }
    func.body.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}
void InstrVisitor::Sema::operator()(Parse::Instr::Br& node){
    index_t index = std::visit(BrIndexVisitor(labelid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Br>(index);
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
void InstrVisitor::Syntax::operator()(Parse::Instr::If& block){
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

std::optional<Parse::Instr::If> Parse::Instr::If::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"if">, Parse::Optional<Token::Id>,
        Parse::Optional<Parse::OneOf<Parse::ValueType, Parse::TypeUse>>,
        Parse::Repeat<Syntax::Instr>,
        Parse::Optional<Parse::Rule<
            Token::Keyword<"else">, Parse::Optional<Token::Id>, Parse::Repeat<Syntax::Instr>
        >>,
        Token::Keyword<"end">, Parse::Optional<Token::Id>
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::If if_instr;
        if_instr.location = std::get<0>(rule).location;
        // id
        auto id = std::get<1>(rule);
        if_instr.id = id ? id->value : "";
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
            if_instr.blocktype.emplace(type);
        }
        // instrs
        for(auto instr : std::get<3>(rule)){
            std::visit(InstrVisitor::Syntax(if_instr.instrs1), instr);
        }
        // else
        auto elsesect = std::get<4>(rule);
        if(elsesect){
            // id
            auto elseid = std::get<1>(elsesect.value());
            if(elseid && !id){
                throw Exception::block_id_mismatch(if_instr.location, std::string(" : else identifier is not present"));
            }else if(id && elseid && (elseid->value != id->value)){
                throw Exception::block_id_mismatch(elseid->location, std::string(" : leading & else id should be the same"));
            }
            // instrs
            for(auto instr : std::get<2>(elsesect.value())){
                std::visit(InstrVisitor::Syntax(if_instr.instrs2), instr);
            }
        }
        // trailing id
        auto trailing = std::get<6>(rule);
        if(trailing && !id){
            throw Exception::block_id_mismatch(if_instr.location, std::string(" : leading identifier is not present"));
        }else if(id && trailing && (trailing->value != id->value)){
            throw Exception::block_id_mismatch(trailing->location, std::string(" : leading & trailing id should be the same"));
        }
        begin = it;
        return if_instr;
    }
    return std::nullopt;
}