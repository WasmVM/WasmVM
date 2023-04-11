// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"
#include "macro.hpp"

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
            return idmap.size() - idmap.at(token.value) - 1;
        }else if(check){
            throw Exception::unknown_identifier(token.location, ": index '" + token.value + "' not found");
        }
        return index_npos;
    }
};

SemaTemplate(Unreachable)
SemaTemplate(Nop)
SemaTemplate(Return)

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Block>(Parse::Instr::Block& node);
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Loop>(Parse::Instr::Loop& node);
template<> void InstrVisitor::Sema::operator()<Parse::Instr::If>(Parse::Instr::If& node);

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Br>(Parse::Instr::Br& node){
    index_t index = std::visit(BrIndexVisitor(labelid_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Br>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Br_if>(Parse::Instr::Br_if& node){
    index_t index = std::visit(BrIndexVisitor(labelid_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Br_if>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Br_table>(Parse::Instr::Br_table& node){
    WasmVM::Instr::Br_table instr;
    for(Parse::Index& index : node.indices){
        instr.indices.emplace_back(std::visit(BrIndexVisitor(labelid_map), index));
    }
    instrs.emplace_back(instr);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Call>(Parse::Instr::Call& node){
    index_t index = std::visit(Parse::Index::Visitor(module.func_indices.id_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Call>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Call_indirect>(Parse::Instr::Call_indirect& node){
    index_t tableidx = node.tableidx ? std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value()) : 0;
    index_t typeidx = Parse::Type::index(node.type, module.module, module.typeid_map, module.paramid_maps);
    instrs.emplace_back(WasmVM::Instr::Call_indirect(tableidx, typeidx));
}

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Block>(Parse::Instr::Block& node){
    WasmVM::Instr::Block instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::TypeUse typeuse = node.blocktype.value();
        if(!typeuse.functype.id_map.empty()){
            Exception::Warning("param ids in block are ignored");
        }
        instr = WasmVM::Instr::Block(Parse::Type::index(typeuse, module.module, module.typeid_map, module.paramid_maps));
    }
    instrs.emplace_back(instr);
    for(Parse::Instr::Instruction instrnode : node.instrs){
        std::visit(InstrVisitor::Sema(module, func, labelid_map, localid_map), instrnode);
    }
    instrs.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Loop>(Parse::Instr::Loop& node){
    WasmVM::Instr::Loop instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::TypeUse typeuse = node.blocktype.value();
        if(!typeuse.functype.id_map.empty()){
            Exception::Warning("param ids in loop are ignored");
        }
        instr = WasmVM::Instr::Loop(Parse::Type::index(typeuse, module.module, module.typeid_map, module.paramid_maps));
    }
    instrs.emplace_back(instr);
    for(Parse::Instr::Instruction instrnode : node.instrs){
        std::visit(InstrVisitor::Sema(module, func, labelid_map, localid_map), instrnode);
    }
    instrs.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::If>(Parse::Instr::If& node){
    WasmVM::Instr::If instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::TypeUse typeuse = node.blocktype.value();
        if(!typeuse.functype.id_map.empty()){
            Exception::Warning("param ids in if are ignored");
        }
        instr = WasmVM::Instr::If(Parse::Type::index(typeuse, module.module, module.typeid_map, module.paramid_maps));
    }
    // Folded
    for(Parse::Instr::Instruction instrnode : node.foldInstrs){
        std::visit(InstrVisitor::Sema(module, func, labelid_map, localid_map), instrnode);
    }
    // Then
    instrs.emplace_back(instr);
    for(Parse::Instr::Instruction instrnode : node.thenInstrs){
        std::visit(InstrVisitor::Sema(module, func, labelid_map, localid_map), instrnode);
    }
    // Else
    if(!node.elseInstrs.empty()){
        instrs.emplace_back(WasmVM::Instr::Else());
        for(Parse::Instr::Instruction instrnode : node.elseInstrs){
            std::visit(InstrVisitor::Sema(module, func, labelid_map, localid_map), instrnode);
        }
    }
    // End
    instrs.emplace_back(WasmVM::Instr::End());
    if(!node.id.empty()){
        labelid_map.erase(node.id);
    }
}


void InstrVisitor::Syntax::operator()(Parse::Instr::Block& node){
    body.emplace_back(node);
}
void InstrVisitor::Syntax::operator()(Parse::Instr::Loop& node){
    body.emplace_back(node);
}
void InstrVisitor::Syntax::operator()(Parse::Instr::If& node){
    body.emplace_back(node);
}

std::optional<Parse::Instr::Block> Parse::Instr::Block::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Parse::Optional<Token::ParenL>,
        Token::Keyword<"block">, Parse::Optional<Token::Id>,
        Parse::TypeUse,
        Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>,
        Parse::OneOf<
            Parse::Rule<Token::Keyword<"end">, Parse::Optional<Token::Id>>,
            Token::ParenR
        >
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Block block;
        block.location = std::get<1>(rule).location;
        // id
        auto id = std::get<2>(rule);
        block.id = id ? id->value : "";
        // blocktype
        block.blocktype.emplace(std::get<3>(rule));
        // instrs
        for(auto instr : std::get<4>(rule)){
            std::visit(overloaded {
                [&](Syntax::Instr& i){
                    std::visit(InstrVisitor::Syntax(block.instrs), i);
                },
                [&](Syntax::FoldedInstr& i){
                    InstrVisitor::Syntax(block.instrs)(i);
                }
            }, instr);
        }
        // trailing id
        auto lparen = std::get<0>(rule);
        auto trailing = std::get<5>(rule);
        if(lparen && std::holds_alternative<Token::ParenR>(trailing)){
            begin = it;
            return block;
        }else if(!lparen && !std::holds_alternative<Token::ParenR>(trailing)){
            auto traling_rule = std::get<0>(trailing);
            auto traling_id = std::get<1>(traling_rule);
            if(traling_id && !id){
                throw Exception::block_id_mismatch(block.location, std::string(" : leading identifier is not present"));
            }else if(id && traling_id && (traling_id->value != id->value)){
                throw Exception::block_id_mismatch(traling_id->location, std::string(" : leading & trailing id should be the same"));
            }
            begin = it;
            return block;
        }
        throw Exception::invalid_folded_instruction(block.location);
    }
    return std::nullopt;
}
std::optional<Parse::Instr::Loop> Parse::Instr::Loop::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Parse::Optional<Token::ParenL>,
        Token::Keyword<"loop">, Parse::Optional<Token::Id>,
        Parse::TypeUse,
        Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>,
        Parse::OneOf<
            Parse::Rule<Token::Keyword<"end">, Parse::Optional<Token::Id>>,
            Token::ParenR
        >
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Loop loop;
        loop.location = std::get<1>(rule).location;
        // id
        auto id = std::get<2>(rule);
        loop.id = id ? id->value : "";
        // blocktype
        loop.blocktype.emplace(std::get<3>(rule));
        // instrs
        for(auto instr : std::get<4>(rule)){
            std::visit(overloaded {
                [&](Syntax::Instr& i){
                    std::visit(InstrVisitor::Syntax(loop.instrs), i);
                },
                [&](Syntax::FoldedInstr& i){
                    InstrVisitor::Syntax(loop.instrs)(i);
                }
            }, instr);
        }
        // trailing id
        auto lparen = std::get<0>(rule);
        auto trailing = std::get<5>(rule);
        if(lparen && std::holds_alternative<Token::ParenR>(trailing)){
            begin = it;
            return loop;
        }else if(!lparen && !std::holds_alternative<Token::ParenR>(trailing)){
            auto traling_rule = std::get<0>(trailing);
            auto traling_id = std::get<1>(traling_rule);
            if(traling_id && !id){
                throw Exception::block_id_mismatch(loop.location, std::string(" : leading identifier is not present"));
            }else if(id && traling_id && (traling_id->value != id->value)){
                throw Exception::block_id_mismatch(traling_id->location, std::string(" : leading & trailing id should be the same"));
            }
            begin = it;
            return loop;
        }
        throw Exception::invalid_folded_instruction(loop.location);
    }
    return std::nullopt;
}
std::optional<Parse::Instr::If> Parse::Instr::If::get(TokenIter& begin, TokenHolder& holder){

    using PlainIfRule = Parse::Rule<Token::Keyword<"if">, Parse::Optional<Token::Id>,
        Parse::TypeUse, Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>, 
        Parse::Optional<Parse::Rule<Token::Keyword<"else">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>>>,
        Token::Keyword<"end">, Parse::Optional<Token::Id>>;
    using FoldedIfRule = Parse::Rule<Token::ParenL, Token::Keyword<"if">, Parse::Optional<Token::Id>,
        Parse::TypeUse, Parse::Repeat<Syntax::FoldedInstr>,
        Token::ParenL, Token::Keyword<"then">, Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>, Token::ParenR,
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"else">, Parse::Repeat<Parse::OneOf<Syntax::Instr, Syntax::FoldedInstr>>, Token::ParenR>>,
        Token::ParenR>;

    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::OneOf<PlainIfRule, FoldedIfRule>::get(it, holder);

    if(syntax){
        Parse::Instr::If if_instr;
        std::visit(overloaded {
            [&](PlainIfRule& rule){
                // id
                auto id = std::get<1>(rule);
                if_instr.id = id ? id->value : "";
                // blocktype
                if_instr.blocktype.emplace(std::get<2>(rule));
                // instrs
                for(auto instr : std::get<3>(rule)){
                    std::visit(overloaded {
                        [&](Syntax::Instr& i){
                            std::visit(InstrVisitor::Syntax(if_instr.thenInstrs), i);
                        },
                        [&](Syntax::FoldedInstr& i){
                            InstrVisitor::Syntax(if_instr.thenInstrs)(i);
                        }
                    }, instr);
                }
                // else
                auto elsesect = std::get<4>(rule);
                if(elsesect){
                    // id
                    auto elseid = std::get<1>(elsesect.value());
                    if(elseid && !id){
                        throw Exception::block_id_mismatch(if_instr.location, std::string(" : leading identifier is not present"));
                    }else if(id && elseid && (elseid->value != id->value)){
                        throw Exception::block_id_mismatch(elseid->location, std::string(" : leading & else id should be the same"));
                    }
                    // instrs
                    for(auto instr : std::get<2>(elsesect.value())){
                        std::visit(overloaded {
                            [&](Syntax::Instr& i){
                                std::visit(InstrVisitor::Syntax(if_instr.elseInstrs), i);
                            },
                            [&](Syntax::FoldedInstr& i){
                                InstrVisitor::Syntax(if_instr.elseInstrs)(i);
                            }
                        }, instr);
                    }
                }
                // trailing id
                auto trailing = std::get<6>(rule);
                if(trailing && !id){
                    throw Exception::block_id_mismatch(if_instr.location, std::string(" : leading identifier is not present"));
                }else if(id && trailing && (trailing->value != id->value)){
                    throw Exception::block_id_mismatch(trailing->location, std::string(" : leading & trailing id should be the same"));
                }
            },
            [&](FoldedIfRule& rule){
                // id
                auto id = std::get<2>(rule);
                if_instr.id = id ? id->value : "";
                // blocktype
                if_instr.blocktype.emplace(std::get<3>(rule));
                // Folded
                auto folded_rules = std::get<4>(rule);
                for(auto& folded_instrs : folded_rules){
                    for(auto& instr : folded_instrs.instrs){
                        std::visit(InstrVisitor::Syntax(if_instr.foldInstrs), instr);
                    }
                }
                // instrs
                for(auto instr : std::get<7>(rule)){
                    std::visit(overloaded {
                        [&](Syntax::Instr& i){
                            std::visit(InstrVisitor::Syntax(if_instr.thenInstrs), i);
                        },
                        [&](Syntax::FoldedInstr& i){
                            InstrVisitor::Syntax(if_instr.thenInstrs)(i);
                        }
                    }, instr);
                }
                // else
                auto elsesect = std::get<9>(rule);
                if(elsesect){
                    // instrs
                    for(auto instr : std::get<2>(elsesect.value())){
                        std::visit(overloaded {
                            [&](Syntax::Instr& i){
                                std::visit(InstrVisitor::Syntax(if_instr.elseInstrs), i);
                            },
                            [&](Syntax::FoldedInstr& i){
                                InstrVisitor::Syntax(if_instr.elseInstrs)(i);
                            }
                        }, instr);
                    }
                }
            }
        }, syntax.value());
        begin = it;
        return if_instr;
    }
    return std::nullopt;
}
std::optional<Parse::Instr::Br_table> Parse::Instr::Br_table::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"br_table">, Parse::Repeat<Parse::Index, 1>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Br_table instr;
        for(auto index : std::get<1>(rule)){
            instr.indices.emplace_back(index);
        }
        begin = it;
        return instr;
    }
    return std::nullopt;
}
std::optional<Parse::Instr::Call_indirect> Parse::Instr::Call_indirect::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"call_indirect">, Parse::Optional<Parse::Index>, Parse::TypeUse
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        begin = it;
        return Parse::Instr::Call_indirect(std::get<1>(rule), std::get<2>(rule));
    }
    return std::nullopt;
}
