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
    block_level += 1;
    std::string label = std::any_cast<std::string>(visitLabel(ctx->label()));
    auto indices = ctx->Id();
    for(auto id : indices){
        if((!id->getText().empty()) && (id->getText() != label)){
            throw Exception::Parse("id '" + id->getText() + "' not match block label", getLocation(id));
        }
    }
    for(auto instr : ctx->instr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitInstr(instr));
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    block_level -= 1;
    if(name == "if"){
        instrs.emplace_back(Instr::Else());
        block_level += 1;
        for(auto instr : ctx->elseinstr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitElseinstr(instr));
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
        block_level -= 1;
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
    block_level += 1;
    visitLabel(ctx->label());
    for(auto instr : ctx->instr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitInstr(instr));
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    block_level -= 1;
    if(name == "if"){
        block_level += 1;
        instrs.emplace_back(Instr::Else());
        for(auto instr : ctx->elseinstr()){
        std::vector<WasmInstr> new_instrs = std::any_cast<std::vector<WasmInstr>>(visitElseinstr(instr));
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
        block_level -= 1;
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

std::any Visitor::visitVariableinstr(WatParser::VariableinstrContext *ctx){
    std::string name = ctx->children[0]->getText();
    if(name.starts_with("local")){
        index_t localidx = std::any_cast<index_t>(visitLocalidx(ctx->localidx()));
        if(name.ends_with("get")){
            return WasmInstr {Instr::Local_get(localidx)};
        }else if(name.ends_with("set")){
            return WasmInstr {Instr::Local_set(localidx)};
        }else{
            return WasmInstr {Instr::Local_tee(localidx)};
        }
    }else{
        index_t globalidx = std::any_cast<index_t>(visitGlobalidx(ctx->globalidx()));
        if(name.ends_with("get")){
            return WasmInstr {Instr::Global_get(globalidx)};
        }else{
            return WasmInstr {Instr::Global_set(globalidx)};
        }
    }
}

std::any Visitor::visitMemoryinstr(WatParser::MemoryinstrContext *ctx){
    std::string name = ctx->children[0]->getText();
    if(name == "data.drop"){
        return WasmInstr {Instr::Data_drop(std::any_cast<index_t>(visitDataidx(ctx->dataidx())))};
    }else if(name == "memory.copy"){
        index_t dst = 0, src = 0;
        if(!ctx->memidx().empty()){
            dst = std::any_cast<index_t>(visitMemidx(ctx->memidx(0)));
            src = std::any_cast<index_t>(visitMemidx(ctx->memidx(1)));
        }
        return WasmInstr {Instr::Memory_copy(dst, src)};
    }
    index_t memidx = 0;
    if(!ctx->memidx().empty()){
        memidx = std::any_cast<index_t>(visitMemidx(ctx->memidx(0)));
    }
    if(name.starts_with("memory")){
        if(name.ends_with("size")){
            return WasmInstr {Instr::Memory_size(memidx)};
        }else if(name.ends_with("grow")){
            return WasmInstr {Instr::Memory_grow(memidx)};
        }else if(name.ends_with("fill")){
            return WasmInstr {Instr::Memory_fill(memidx)};
        }else{
            return WasmInstr {Instr::Memory_init(memidx, std::any_cast<index_t>(visitDataidx(ctx->dataidx())))};
        }
    }
    auto memarg = std::any_cast<std::pair<offset_t, std::optional<align_t>>>(visitMemarg(ctx->memarg()));
    if(!memarg.second.has_value()){
        if(name.ends_with("8") || name.ends_with("8_s") || name.ends_with("8_u")){
            memarg.second = 0;
        }else if(name.ends_with("16") || name.ends_with("16_s") || name.ends_with("16_u")){
            memarg.second = 1;
        }else if(name.ends_with("32") || name.ends_with("32_s") || name.ends_with("32_u")){
            memarg.second = 2;
        }
    }
    if(name.starts_with("i32")){
        if(!memarg.second.has_value()){
            memarg.second = 2;
        }
        if(name.ends_with("load")){
            return WasmInstr {Instr::I32_load(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load8_s")){
            return WasmInstr {Instr::I32_load8_s(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load8_u")){
            return WasmInstr {Instr::I32_load8_u(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load16_s")){
            return WasmInstr {Instr::I32_load16_s(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load16_u")){
            return WasmInstr {Instr::I32_load16_u(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("store")){
            return WasmInstr {Instr::I32_store(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("store8")){
            return WasmInstr {Instr::I32_store8(memidx, memarg.first, memarg.second.value())};
        }else{
            return WasmInstr {Instr::I32_store16(memidx, memarg.first, memarg.second.value())};
        }
    }else if(name.starts_with("i64")){
        if(!memarg.second.has_value()){
            memarg.second = 3;
        }
        if(name.ends_with("load")){
            return WasmInstr {Instr::I64_load(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load8_s")){
            return WasmInstr {Instr::I64_load8_s(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load8_u")){
            return WasmInstr {Instr::I64_load8_u(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load16_s")){
            return WasmInstr {Instr::I64_load16_s(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load16_u")){
            return WasmInstr {Instr::I64_load16_u(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load32_s")){
            return WasmInstr {Instr::I64_load32_s(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("load32_u")){
            return WasmInstr {Instr::I64_load32_u(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("store")){
            return WasmInstr {Instr::I64_store(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("store8")){
            return WasmInstr {Instr::I64_store8(memidx, memarg.first, memarg.second.value())};
        }else if(name.ends_with("store16")){
            return WasmInstr {Instr::I64_store16(memidx, memarg.first, memarg.second.value())};
        }else{
            return WasmInstr {Instr::I64_store32(memidx, memarg.first, memarg.second.value())};
        }
    }else if(name.starts_with("f32")){
        if(!memarg.second.has_value()){
            memarg.second = 2;
        }
        if(name.ends_with("load")){
            return WasmInstr {Instr::F32_load(memidx, memarg.first, memarg.second.value())};
        }else{
            return WasmInstr {Instr::F32_store(memidx, memarg.first, memarg.second.value())};
        }
    }else{
        if(!memarg.second.has_value()){
            memarg.second = 3;
        }
        if(name.ends_with("load")){
            return WasmInstr {Instr::F64_load(memidx, memarg.first, memarg.second.value())};
        }else{
            return WasmInstr {Instr::F64_store(memidx, memarg.first, memarg.second.value())};
        }
    }
}

std::any Visitor::visitNumericinstr(WatParser::NumericinstrContext *ctx){
    std::string name = ctx->children[0]->getText();
    if(name.ends_with("const")){
        if(name.starts_with("i32")){
            return WasmInstr {Instr::I32_const(std::any_cast<i32_t>(visitI32(ctx->i32())))};
        }else if(name.starts_with("i64")){
            return WasmInstr {Instr::I64_const(std::any_cast<i64_t>(visitI64(ctx->i64())))};
        }else if(name.starts_with("f32")){
            return WasmInstr {Instr::F32_const(std::any_cast<f32_t>(visitF32(ctx->f32())))};
        }else{
            return WasmInstr {Instr::F64_const(std::any_cast<f64_t>(visitF64(ctx->f64())))};
        }
    }
    if(name.starts_with("i32")){
        if(name.ends_with("clz")){
            return WasmInstr {Instr::I32_clz()};
        }else if(name.ends_with("ctz")){
            return WasmInstr {Instr::I32_ctz()};
        }else if(name.ends_with("popcnt")){
            return WasmInstr {Instr::I32_popcnt()};
        }else if(name.ends_with("add")){
            return WasmInstr {Instr::I32_add()};
        }else if(name.ends_with("sub")){
            return WasmInstr {Instr::I32_sub()};
        }else if(name.ends_with("mul")){
            return WasmInstr {Instr::I32_mul()};
        }else if(name.ends_with("div_s")){
            return WasmInstr {Instr::I32_div_s()};
        }else if(name.ends_with("div_u")){
            return WasmInstr {Instr::I32_div_u()};
        }else if(name.ends_with("rem_s")){
            return WasmInstr {Instr::I32_rem_s()};
        }else if(name.ends_with("rem_u")){
            return WasmInstr {Instr::I32_rem_u()};
        }else if(name.ends_with("and")){
            return WasmInstr {Instr::I32_and()};
        }else if(name.ends_with("xor")){
            return WasmInstr {Instr::I32_xor()};
        }else if(name.ends_with("or")){
            return WasmInstr {Instr::I32_or()};
        }else if(name.ends_with("shl")){
            return WasmInstr {Instr::I32_shl()};
        }else if(name.ends_with("shr_s")){
            return WasmInstr {Instr::I32_shr_s()};
        }else if(name.ends_with("shr_u")){
            return WasmInstr {Instr::I32_shr_u()};
        }else if(name.ends_with("rotl")){
            return WasmInstr {Instr::I32_rotl()};
        }else if(name.ends_with("rotr")){
            return WasmInstr {Instr::I32_rotr()};
        }else if(name.ends_with("eqz")){
            return WasmInstr {Instr::I32_eqz()};
        }else if(name.ends_with("eq")){
            return WasmInstr {Instr::I32_eq()};
        }else if(name.ends_with("ne")){
            return WasmInstr {Instr::I32_ne()};
        }else if(name.ends_with("lt_s")){
            return WasmInstr {Instr::I32_lt_s()};
        }else if(name.ends_with("lt_u")){
            return WasmInstr {Instr::I32_lt_u()};
        }else if(name.ends_with("gt_s")){
            return WasmInstr {Instr::I32_gt_s()};
        }else if(name.ends_with("gt_u")){
            return WasmInstr {Instr::I32_gt_u()};
        }else if(name.ends_with("le_s")){
            return WasmInstr {Instr::I32_le_s()};
        }else if(name.ends_with("le_u")){
            return WasmInstr {Instr::I32_le_u()};
        }else if(name.ends_with("ge_s")){
            return WasmInstr {Instr::I32_ge_s()};
        }else if(name.ends_with("ge_u")){
            return WasmInstr {Instr::I32_ge_u()};
        }else if(name.ends_with("wrap_i64")){
            return WasmInstr {Instr::I32_wrap_i64()};
        }else if(name.ends_with("trunc_f32_s")){
            return WasmInstr {Instr::I32_trunc_f32_s()};
        }else if(name.ends_with("trunc_f32_u")){
            return WasmInstr {Instr::I32_trunc_f32_u()};
        }else if(name.ends_with("trunc_f64_s")){
            return WasmInstr {Instr::I32_trunc_f64_s()};
        }else if(name.ends_with("trunc_f64_u")){
            return WasmInstr {Instr::I32_trunc_f64_u()};
        }else if(name.ends_with("trunc_sat_f32_s")){
            return WasmInstr {Instr::I32_trunc_sat_f32_s()};
        }else if(name.ends_with("trunc_sat_f32_u")){
            return WasmInstr {Instr::I32_trunc_sat_f32_u()};
        }else if(name.ends_with("trunc_sat_f64_s")){
            return WasmInstr {Instr::I32_trunc_sat_f64_s()};
        }else if(name.ends_with("trunc_sat_f64_u")){
            return WasmInstr {Instr::I32_trunc_sat_f64_u()};
        }else if(name.ends_with("reinterpret_f32")){
            return WasmInstr {Instr::I32_reinterpret_f32()};
        }else if(name.ends_with("extend8_s")){
            return WasmInstr {Instr::I32_extend8_s()};
        }else{
            return WasmInstr {Instr::I32_extend16_s()};
        }
    }else if(name.starts_with("i64")){
        if(name.ends_with("clz")){
            return WasmInstr {Instr::I64_clz()};
        }else if(name.ends_with("ctz")){
            return WasmInstr {Instr::I64_ctz()};
        }else if(name.ends_with("popcnt")){
            return WasmInstr {Instr::I64_popcnt()};
        }else if(name.ends_with("add")){
            return WasmInstr {Instr::I64_add()};
        }else if(name.ends_with("sub")){
            return WasmInstr {Instr::I64_sub()};
        }else if(name.ends_with("mul")){
            return WasmInstr {Instr::I64_mul()};
        }else if(name.ends_with("div_s")){
            return WasmInstr {Instr::I64_div_s()};
        }else if(name.ends_with("div_u")){
            return WasmInstr {Instr::I64_div_u()};
        }else if(name.ends_with("rem_s")){
            return WasmInstr {Instr::I64_rem_s()};
        }else if(name.ends_with("rem_u")){
            return WasmInstr {Instr::I64_rem_u()};
        }else if(name.ends_with("and")){
            return WasmInstr {Instr::I64_and()};
        }else if(name.ends_with("xor")){
            return WasmInstr {Instr::I64_xor()};
        }else if(name.ends_with("or")){
            return WasmInstr {Instr::I64_or()};
        }else if(name.ends_with("shl")){
            return WasmInstr {Instr::I64_shl()};
        }else if(name.ends_with("shr_s")){
            return WasmInstr {Instr::I64_shr_s()};
        }else if(name.ends_with("shr_u")){
            return WasmInstr {Instr::I64_shr_u()};
        }else if(name.ends_with("rotl")){
            return WasmInstr {Instr::I64_rotl()};
        }else if(name.ends_with("rotr")){
            return WasmInstr {Instr::I64_rotr()};
        }else if(name.ends_with("eqz")){
            return WasmInstr {Instr::I64_eqz()};
        }else if(name.ends_with("eq")){
            return WasmInstr {Instr::I64_eq()};
        }else if(name.ends_with("ne")){
            return WasmInstr {Instr::I64_ne()};
        }else if(name.ends_with("lt_s")){
            return WasmInstr {Instr::I64_lt_s()};
        }else if(name.ends_with("lt_u")){
            return WasmInstr {Instr::I64_lt_u()};
        }else if(name.ends_with("gt_s")){
            return WasmInstr {Instr::I64_gt_s()};
        }else if(name.ends_with("gt_u")){
            return WasmInstr {Instr::I64_gt_u()};
        }else if(name.ends_with("le_s")){
            return WasmInstr {Instr::I64_le_s()};
        }else if(name.ends_with("le_u")){
            return WasmInstr {Instr::I64_le_u()};
        }else if(name.ends_with("ge_s")){
            return WasmInstr {Instr::I64_ge_s()};
        }else if(name.ends_with("ge_u")){
            return WasmInstr {Instr::I64_ge_u()};
        }else if(name.ends_with("extend_i32_s")){
            return WasmInstr {Instr::I64_extend_i32_s()};
        }else if(name.ends_with("extend_i32_u")){
            return WasmInstr {Instr::I64_extend_i32_u()};
        }else if(name.ends_with("trunc_f32_s")){
            return WasmInstr {Instr::I64_trunc_f32_s()};
        }else if(name.ends_with("trunc_f32_u")){
            return WasmInstr {Instr::I64_trunc_f32_u()};
        }else if(name.ends_with("trunc_f64_s")){
            return WasmInstr {Instr::I64_trunc_f64_s()};
        }else if(name.ends_with("trunc_f64_u")){
            return WasmInstr {Instr::I64_trunc_f64_u()};
        }else if(name.ends_with("trunc_sat_f32_s")){
            return WasmInstr {Instr::I64_trunc_sat_f32_s()};
        }else if(name.ends_with("trunc_sat_f32_u")){
            return WasmInstr {Instr::I64_trunc_sat_f32_u()};
        }else if(name.ends_with("trunc_sat_f64_s")){
            return WasmInstr {Instr::I64_trunc_sat_f64_s()};
        }else if(name.ends_with("trunc_sat_f64_u")){
            return WasmInstr {Instr::I64_trunc_sat_f64_u()};
        }else if(name.ends_with("extend8_s")){
            return WasmInstr {Instr::I64_extend8_s()};
        }else if(name.ends_with("extend16_s")){
            return WasmInstr {Instr::I64_extend16_s()};
        }else{
            return WasmInstr {Instr::I64_extend32_s()};
        }
    }else if(name.starts_with("f32")){
        if(name.ends_with("abs")){
            return WasmInstr {Instr::F32_abs()};
        }else if(name.ends_with("neg")){
            return WasmInstr {Instr::F32_neg()};
        }else if(name.ends_with("ceil")){
            return WasmInstr {Instr::F32_ceil()};
        }else if(name.ends_with("floor")){
            return WasmInstr {Instr::F32_floor()};
        }else if(name.ends_with("trunc")){
            return WasmInstr {Instr::F32_trunc()};
        }else if(name.ends_with("nearest")){
            return WasmInstr {Instr::F32_nearest()};
        }else if(name.ends_with("sqrt")){
            return WasmInstr {Instr::F32_sqrt()};
        }else if(name.ends_with("add")){
            return WasmInstr {Instr::F32_add()};
        }else if(name.ends_with("sub")){
            return WasmInstr {Instr::F32_sub()};
        }else if(name.ends_with("mul")){
            return WasmInstr {Instr::F32_mul()};
        }else if(name.ends_with("div")){
            return WasmInstr {Instr::F32_div()};
        }else if(name.ends_with("min")){
            return WasmInstr {Instr::F32_min()};
        }else if(name.ends_with("max")){
            return WasmInstr {Instr::F32_max()};
        }else if(name.ends_with("copysign")){
            return WasmInstr {Instr::F32_copysign()};
        }else if(name.ends_with("eq")){
            return WasmInstr {Instr::F32_eq()};
        }else if(name.ends_with("ne")){
            return WasmInstr {Instr::F32_ne()};
        }else if(name.ends_with("lt")){
            return WasmInstr {Instr::F32_lt()};
        }else if(name.ends_with("gt")){
            return WasmInstr {Instr::F32_gt()};
        }else if(name.ends_with("le")){
            return WasmInstr {Instr::F32_le()};
        }else if(name.ends_with("ge")){
            return WasmInstr {Instr::F32_ge()};
        }else if(name.ends_with("convert_i32_s")){
            return WasmInstr {Instr::F32_convert_i32_s()};
        }else if(name.ends_with("convert_i32_u")){
            return WasmInstr {Instr::F32_convert_i32_u()};
        }else if(name.ends_with("convert_i64_s")){
            return WasmInstr {Instr::F32_convert_i64_s()};
        }else if(name.ends_with("convert_i64_u")){
            return WasmInstr {Instr::F32_convert_i64_u()};
        }else{
            return WasmInstr {Instr::F32_demote_f64()};
        }
    }else{
        if(name.ends_with("abs")){
            return WasmInstr {Instr::F64_abs()};
        }else if(name.ends_with("neg")){
            return WasmInstr {Instr::F64_neg()};
        }else if(name.ends_with("ceil")){
            return WasmInstr {Instr::F64_ceil()};
        }else if(name.ends_with("floor")){
            return WasmInstr {Instr::F64_floor()};
        }else if(name.ends_with("trunc")){
            return WasmInstr {Instr::F64_trunc()};
        }else if(name.ends_with("nearest")){
            return WasmInstr {Instr::F64_nearest()};
        }else if(name.ends_with("sqrt")){
            return WasmInstr {Instr::F64_sqrt()};
        }else if(name.ends_with("add")){
            return WasmInstr {Instr::F64_add()};
        }else if(name.ends_with("sub")){
            return WasmInstr {Instr::F64_sub()};
        }else if(name.ends_with("mul")){
            return WasmInstr {Instr::F64_mul()};
        }else if(name.ends_with("div")){
            return WasmInstr {Instr::F64_div()};
        }else if(name.ends_with("min")){
            return WasmInstr {Instr::F64_min()};
        }else if(name.ends_with("max")){
            return WasmInstr {Instr::F64_max()};
        }else if(name.ends_with("copysign")){
            return WasmInstr {Instr::F64_copysign()};
        }else if(name.ends_with("eq")){
            return WasmInstr {Instr::F64_eq()};
        }else if(name.ends_with("ne")){
            return WasmInstr {Instr::F64_ne()};
        }else if(name.ends_with("lt")){
            return WasmInstr {Instr::F64_lt()};
        }else if(name.ends_with("gt")){
            return WasmInstr {Instr::F64_gt()};
        }else if(name.ends_with("le")){
            return WasmInstr {Instr::F64_le()};
        }else if(name.ends_with("ge")){
            return WasmInstr {Instr::F64_ge()};
        }else if(name.ends_with("convert_i32_s")){
            return WasmInstr {Instr::F64_convert_i32_s()};
        }else if(name.ends_with("convert_i32_u")){
            return WasmInstr {Instr::F64_convert_i32_u()};
        }else if(name.ends_with("convert_i64_s")){
            return WasmInstr {Instr::F64_convert_i64_s()};
        }else if(name.ends_with("convert_i64_u")){
            return WasmInstr {Instr::F64_convert_i64_u()};
        }else if(name.ends_with("promote_f32")){
            return WasmInstr {Instr::F64_promote_f32()};
        }else{
            return WasmInstr {Instr::F64_reinterpret_i64()};
        }
    }
}