// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"
#include "macro.hpp"

using namespace WasmVM;

MemSema(I32_load)
MemSema(I64_load)
MemSema(F32_load)
MemSema(F64_load)
MemSema(I32_load8_s)
MemSema(I32_load8_u)
MemSema(I32_load16_s)
MemSema(I32_load16_u)
MemSema(I64_load8_s)
MemSema(I64_load8_u)
MemSema(I64_load16_s)
MemSema(I64_load16_u)
MemSema(I64_load32_s)
MemSema(I64_load32_u)
MemSema(I32_store)
MemSema(I64_store)
MemSema(F32_store)
MemSema(F64_store)
MemSema(I32_store8)
MemSema(I32_store16)
MemSema(I64_store8)
MemSema(I64_store16)
MemSema(I64_store32)

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Memory_size>(Parse::Instr::Memory_size& node){
    if(node.memidx){
        index_t index = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.memidx.value());
        func.body.emplace_back<WasmVM::Instr::Memory_size>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Memory_size>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Memory_grow>(Parse::Instr::Memory_grow& node){
    if(node.memidx){
        index_t index = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.memidx.value());
        func.body.emplace_back<WasmVM::Instr::Memory_grow>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Memory_grow>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Memory_fill>(Parse::Instr::Memory_fill& node){
    if(node.memidx){
        index_t index = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.memidx.value());
        func.body.emplace_back<WasmVM::Instr::Memory_fill>(index);
    }else{
        func.body.emplace_back<WasmVM::Instr::Memory_fill>(0);
    }
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Memory_copy>(Parse::Instr::Memory_copy& node){
    index_t dstidx = 0;
    index_t srcidx = 0;
    if(node.dstidx){
        dstidx = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.dstidx.value());
    }
    if(node.srcidx){
        srcidx = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.srcidx.value());
    }
    func.body.emplace_back(WasmVM::Instr::Memory_copy(dstidx, srcidx));
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Memory_init>(Parse::Instr::Memory_init& node){
    index_t memidx = 0;
    if(node.memidx){
        memidx = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.memidx.value());
    }
    func.body.emplace_back(WasmVM::Instr::Memory_init(memidx, std::visit(Parse::Index::Visitor(module.dataid_map), node.dataidx)));
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Data_drop>(Parse::Instr::Data_drop& node){
    index_t index = std::visit(Parse::Index::Visitor(module.dataid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Data_drop>(index);
}

std::optional<Parse::Instr::Memory_copy> Parse::Instr::Memory_copy::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"memory.copy">, Parse::Optional<Parse::Rule<Parse::Index, Parse::Index>>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        auto indices = std::get<1>(rule);
        begin = it;
        if(indices){
            return Parse::Instr::Memory_copy(std::get<0>(indices.value()), std::get<1>(indices.value()));
        }else{
            return Parse::Instr::Memory_copy(std::nullopt, std::nullopt);
        }
    }
    return std::nullopt;
}
std::optional<Parse::Instr::Memory_init> Parse::Instr::Memory_init::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;
    auto syntax = Parse::Rule<
        Token::Keyword<"memory.init">, Parse::Index, Parse::Optional<Parse::Index>
    >::get(it, holder);

    if(syntax){
        auto rule = syntax.value();
        auto index1 = std::get<1>(rule);
        auto index2 = std::get<2>(rule);
        begin = it;
        if(index2){
            return Parse::Instr::Memory_init(index1, index2.value());
        }else{
            return Parse::Instr::Memory_init(std::nullopt, index1);
        }
    }
    return std::nullopt;
}
