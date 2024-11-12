// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>

using namespace WasmVM;

std::any Visitor::visitElseinstr(WatParser::ElseinstrContext *ctx){
    return visitInstr(ctx->instr());
}

std::any Visitor::visitInstr(WatParser::InstrContext *ctx){
    if(ctx->plaininstr() != nullptr){
        return std::vector<WasmInstr> {std::any_cast<WasmInstr>(visitPlaininstr(ctx->plaininstr()))};
    }else{
        return visitChildren(ctx);
    }
}

std::any Visitor::visitPlaininstr(WatParser::PlaininstrContext *ctx){
    return visitChildren(ctx);
}

std::any Visitor::visitControlinstr(WatParser::ControlinstrContext *ctx){
    std::string name = ctx->children[0]->getText();
    if(name == "unreachable"){
        return WasmInstr {Instr::Unreachable()};
    }else if(name == "nop"){
        return WasmInstr {Instr::Nop()};
    }else if(name == "br"){
        return WasmInstr {Instr::Br(std::any_cast<index_t>(visitLabelidx(ctx->labelidx(0))))};
    }else if(name == "br_if"){
        return WasmInstr {Instr::Br_if(std::any_cast<index_t>(visitLabelidx(ctx->labelidx(0))))};
    }else if(name == "br_table"){
        Instr::Br_table instr;
        auto labels = ctx->labelidx();
        std::transform(labels.begin(), labels.end(), std::back_inserter(instr.indices), 
            [&](WasmVM::WatParser::LabelidxContext* label){
                return std::any_cast<index_t>(visitLabelidx(label));
            }
        );
        return WasmInstr {instr};
    }else if(name == "return"){
        return WasmInstr {Instr::Return()};
    }else if(name == "call"){
        return WasmInstr {Instr::Call(std::any_cast<index_t>(visitFuncidx(ctx->funcidx())))};
    }else{
        return WasmInstr {Instr::Call_indirect(
            (ctx->tableidx() != nullptr) ? std::any_cast<index_t>(visitTableidx(ctx->tableidx())) : 0,
            std::any_cast<index_t>(visitTypeuse(ctx->typeuse()))
        )};
    }
}

std::any Visitor::visitBlockinstr(WatParser::BlockinstrContext *ctx){
    std::string label = std::any_cast<std::string>(visitLabel(ctx->label()));
    auto indices = ctx->Id();
    for(auto id : indices){
        if((!id->getText().empty()) && (id->getText() != label)){
            throw Exception::Parse("id '" + id->getText() + "' not match block label", getLocation(id));
        }
    }
    std::optional<index_t> type;
    if(!ctx->typeuse()->children.empty()){
        type = std::any_cast<index_t>(visitTypeuse(ctx->typeuse()));
    }
    std::vector<WasmInstr> instrs;
    std::string name = ctx->children[0]->getText();
    if(name == "if"){
        instrs.emplace_back(Instr::If(type));
    }else if(name == "block"){
        instrs.emplace_back(Instr::Block(type));
    }else{
        instrs.emplace_back(Instr::Loop(type));
    }
    for(auto instr : ctx->instr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitInstr(instr));
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    if(name == "if"){
        instrs.emplace_back(Instr::Else());
        for(auto instr : ctx->elseinstr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitElseinstr(instr));
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
    }
    instrs.emplace_back(Instr::End());
    return instrs;
}


std::any Visitor::visitFoldedinstr(WatParser::FoldedinstrContext *ctx){
    std::vector<WasmInstr> instrs;
    for(auto folded : ctx->foldedinstr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitFoldedinstr(folded));
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    if(ctx->plaininstr() != nullptr){
        instrs.emplace_back(std::any_cast<WasmInstr>(visitPlaininstr(ctx->plaininstr())));
        return instrs;
    }
    std::string label = std::any_cast<std::string>(visitLabel(ctx->label()));
    std::optional<index_t> type;
    if(!ctx->typeuse()->children.empty()){
        type = std::any_cast<index_t>(visitTypeuse(ctx->typeuse()));
    }
    std::string name = ctx->children[1]->getText();
    if(name == "if"){
        instrs.emplace_back(Instr::If(type));
    }else if(name == "block"){
        instrs.emplace_back(Instr::Block(type));
    }else{
        instrs.emplace_back(Instr::Loop(type));
    }
    for(auto instr : ctx->instr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitInstr(instr));
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    if(name == "if"){
        instrs.emplace_back(Instr::Else());
        for(auto instr : ctx->elseinstr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitElseinstr(instr));
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
    }
    instrs.emplace_back(Instr::End());
    return instrs;
}

std::any Visitor::visitReferenceinstr(WatParser::ReferenceinstrContext *ctx){
    if(ctx->RefNull() != nullptr){
        if(ctx->children.back()->getText().ends_with("func")){
            return WasmInstr {Instr::Ref_null(RefType::funcref)};
        }else{
            return WasmInstr {Instr::Ref_null(RefType::externref)};
        }
    }else if(ctx->children[0]->getText() == "ref.is_null"){
        return WasmInstr {Instr::Ref_is_null()};
    }else{
        return WasmInstr {Instr::Ref_func(std::any_cast<index_t>(visitFuncidx(ctx->funcidx())))};
    }
}

std::any Visitor::visitParametricinstr(WatParser::ParametricinstrContext *ctx){
    if(ctx->children[0]->getText() == "select"){
        Instr::Select instr;
        if(!ctx->result().empty()){
            for(auto result : ctx->result()){
                std::vector<ValueType> new_results = std::any_cast<std::vector<ValueType>>(visitResult(result));
                instr.valtypes.insert(instr.valtypes.end(), new_results.begin(), new_results.end());
            }
        }
        return WasmInstr {instr};
    }else{
        return WasmInstr {Instr::Drop()};
    }
}

std::any Visitor::visitTableinstr(WatParser::TableinstrContext *ctx){
    std::string name = ctx->children[0]->getText();
    if(name == "elem.drop"){
        return WasmInstr {Instr::Elem_drop(std::any_cast<index_t>(visitElemidx(ctx->elemidx())))};
    }else if(name == "table.copy"){
        index_t dst = 0, src = 0;
        if(!ctx->tableidx().empty()){
            dst = std::any_cast<index_t>(visitTableidx(ctx->tableidx(0)));
            src = std::any_cast<index_t>(visitTableidx(ctx->tableidx(1)));
        }
        return WasmInstr {Instr::Table_copy(dst, src)};
    }
    index_t tableidx = 0;
    if(!ctx->tableidx().empty()){
        tableidx = std::any_cast<index_t>(visitTableidx(ctx->tableidx(0)));
    }
    if(name == "table.get"){
        return WasmInstr {Instr::Table_get(tableidx)};
    }else if(name == "table.set"){
        return WasmInstr {Instr::Table_set(tableidx)};
    }else if(name == "table.size"){
        return WasmInstr {Instr::Table_size(tableidx)};
    }else if(name == "table.grow"){
        return WasmInstr {Instr::Table_grow(tableidx)};
    }else if(name == "table.fill"){
        return WasmInstr {Instr::Table_fill(tableidx)};
    }else{
        return WasmInstr {Instr::Table_init(tableidx, std::any_cast<index_t>(visitElemidx(ctx->elemidx())))};
    }
}