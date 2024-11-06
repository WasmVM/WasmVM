// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>

using namespace WasmVM;

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