// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"

using namespace WasmVM;

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_get>(Parse::Instr::Table_get& node){
    if(node.tableidx){
        index_t index = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
        func.body.emplace_back<WasmVM::Instr::Table_get>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Table_get>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_set>(Parse::Instr::Table_set& node){
    if(node.tableidx){
        index_t index = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
        func.body.emplace_back<WasmVM::Instr::Table_set>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Table_set>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_size>(Parse::Instr::Table_size& node){
    if(node.tableidx){
        index_t index = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
        func.body.emplace_back<WasmVM::Instr::Table_size>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Table_size>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_grow>(Parse::Instr::Table_grow& node){
    if(node.tableidx){
        index_t index = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
        func.body.emplace_back<WasmVM::Instr::Table_grow>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Table_grow>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_fill>(Parse::Instr::Table_fill& node){
    if(node.tableidx){
        index_t index = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
        func.body.emplace_back<WasmVM::Instr::Table_fill>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Table_fill>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_copy>(Parse::Instr::Table_copy& node){
    index_t dstidx = 0;
    index_t srcidx = 0;
    if(node.dstidx){
        dstidx = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.dstidx.value());
    }
    if(node.srcidx){
        srcidx = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.srcidx.value());
    }
    func.body.emplace_back(WasmVM::Instr::Table_copy(dstidx, srcidx));
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Table_init>(Parse::Instr::Table_init& node){
    index_t tableidx = 0;
    if(node.tableidx){
        tableidx = std::visit(Parse::Index::Visitor(module.table_indices.id_map), node.tableidx.value());
    }
    func.body.emplace_back(WasmVM::Instr::Table_init(tableidx, std::visit(Parse::Index::Visitor(module.elemid_map), node.elemidx)));
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Elem_drop>(Parse::Instr::Elem_drop& node){
    index_t index = std::visit(Parse::Index::Visitor(module.elemid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Elem_drop>(index);
}

std::optional<Parse::Instr::Table_copy> Parse::Instr::Table_copy::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"table.copy">, Parse::Optional<Parse::Rule<Parse::Index, Parse::Index>>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        auto indices = std::get<1>(rule);
        begin = it;
        if(indices){
            return Parse::Instr::Table_copy(std::get<0>(indices.value()), std::get<1>(indices.value()));
        }else{
            return Parse::Instr::Table_copy(std::nullopt, std::nullopt);
        }
    }
    return std::nullopt;
}
std::optional<Parse::Instr::Table_init> Parse::Instr::Table_init::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"table.init">, Parse::Index, Parse::Optional<Parse::Index>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        auto index1 = std::get<1>(rule);
        auto index2 = std::get<2>(rule);
        begin = it;
        if(index2){
            return Parse::Instr::Table_init(index1, index2.value());
        }else{
            return Parse::Instr::Table_init(std::nullopt, index1);
        }
    }
    return std::nullopt;
}

#define table_template(Kwd) \
template<> std::optional<Parse::Instr::TableInstr<Kwd>> Parse::Instr::TableInstr<Kwd>::get(TokenIter& begin, TokenHolder& holder){ \
    std::list<TokenType>::iterator it = begin; \
    auto syntax = Parse::Rule<Token::Keyword<Kwd>, Parse::Optional<Index>>::get(it, holder); \
    if(syntax){ \
        begin = it; \
        return Parse::Instr::TableInstr<Kwd>(std::get<1>(syntax.value())); \
    } \
    return std::nullopt; \
}

table_template("table.get")
table_template("table.set")
table_template("table.size")
table_template("table.grow")
table_template("table.fill")