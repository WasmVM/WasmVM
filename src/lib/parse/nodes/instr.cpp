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
    WasmVM::Instr::Block instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::Type type(node.blocktype.value());
        if(!type.func.id_map.empty()){
            Exception::Warning("param ids in block are ignored");
        }
        instr = WasmVM::Instr::Block(type.index(module.module, module.typeid_map, module.paramid_maps));
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
    WasmVM::Instr::Loop instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::Type type(node.blocktype.value());
        if(!type.func.id_map.empty()){
            Exception::Warning("param ids in loop are ignored");
        }
        instr = WasmVM::Instr::Loop(type.index(module.module, module.typeid_map, module.paramid_maps));
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
    WasmVM::Instr::If instr;
    if(!node.id.empty()){
        if(labelid_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, " : block id '" + node.id + "' is used");
        }
        labelid_map[node.id] = labelid_map.size();
    }
    if(node.blocktype){
        Parse::Type type(node.blocktype.value());
        if(!type.func.id_map.empty()){
            Exception::Warning("param ids in if are ignored");
        }
        instr = WasmVM::Instr::If(type.index(module.module, module.typeid_map, module.paramid_maps));
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
void InstrVisitor::Sema::operator()(Parse::Instr::Br_if& node){
    index_t index = std::visit(BrIndexVisitor(labelid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Br_if>(index);
}
void InstrVisitor::Sema::operator()(Parse::Instr::Br_table& node){
    WasmVM::Instr::Br_table instr;
    for(Parse::Index& index : node.indices){
        instr.indices.emplace_back(std::visit(BrIndexVisitor(labelid_map), index));
    }
    func.body.emplace_back(instr);
}
void InstrVisitor::Sema::operator()(Parse::Instr::Call_indirect& node){
    index_t tableidx = node.tableidx ? std::visit(Parse::Index::Visitor(module.tableid_map), node.tableidx.value()) : 0;
    index_t typeidx = Parse::Type(node.type).index(module.module, module.typeid_map, module.paramid_maps);
    func.body.emplace_back(WasmVM::Instr::Call_indirect(tableidx, typeidx));
}
void InstrVisitor::Sema::operator()(Parse::Instr::Ref_func& node){
    index_t index = std::visit(Parse::Index::Visitor(module.funcid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Ref_func>(index);
}

void InstrVisitor::Syntax::operator()(WasmVM::Syntax::PlainInstr& plain){
    std::visit(overloaded {
        [&](auto&& instr){
            body.emplace_back(instr);
        }
    }, plain);
}

std::optional<Parse::Instr::Block> Parse::Instr::Block::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"block">, Parse::Optional<Token::Id>,
        Parse::TypeUse,
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
        block.blocktype.emplace(std::get<2>(rule));
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
        Parse::TypeUse,
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
        loop.blocktype.emplace(std::get<2>(rule));
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
        Parse::TypeUse,
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
        if_instr.blocktype.emplace(std::get<2>(rule));
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

std::optional<Parse::Instr::Br_table> Parse::Instr::Br_table::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"br_table">, Parse::Repeat<Parse::Index, 1>
    >::get(it, end);

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

std::optional<Parse::Instr::Call_indirect> Parse::Instr::Call_indirect::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"call_indirect">, Parse::Optional<Parse::Index>, Parse::TypeUse
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        begin = it;
        return Parse::Instr::Call_indirect(std::get<1>(rule), std::get<2>(rule));
    }
    return std::nullopt;
}

std::optional<Parse::Instr::Ref_null> Parse::Instr::Ref_null::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"ref.null">, Parse::OneOf<Token::Keyword<"func">, Token::Keyword<"extern">>
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        begin = it;
        return Parse::Instr::Ref_null(std::visit(overloaded {
            [](Token::Keyword<"func">){
                return WasmVM::RefType::funcref;
            },
            [](Token::Keyword<"extern">){
                return WasmVM::RefType::externref;
            },
        }, std::get<1>(rule)));
    }
    return std::nullopt;
}

std::optional<Parse::Instr::Select> Parse::Instr::Select::get(TokenIter& begin, const TokenIter& end){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"select">, 
        Repeat<
            Rule<Token::ParenL, Token::Keyword<"result">, Repeat<ValueType>, Token::ParenR>
        >
    >::get(it, end);

    if(syntax){
        auto rule = syntax.value();
        Parse::Instr::Select instr;
        for(auto result : std::get<1>(rule)){
            for(auto type : std::get<2>(result)){
                instr.valtypes.emplace_back(type);
            }
        }
        begin = it;
        return instr;
    }
    return std::nullopt;
}