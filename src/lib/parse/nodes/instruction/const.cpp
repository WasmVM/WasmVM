// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"

using namespace WasmVM;


void InstrVisitor::ConstSema::operator()(Parse::Instr::Ref_null& instr){
    instrs.emplace_back(instr);
}
void InstrVisitor::ConstSema::operator()(Parse::Instr::Ref_func& instr){
    index_t index = std::visit(Parse::Index::Visitor(module.func_indices.id_map), instr.index);
    instrs.emplace_back<WasmVM::Instr::Ref_func>(index);
}
void InstrVisitor::ConstSema::operator()(Parse::Instr::Global_get& instr){
    index_t index = std::visit(Parse::Index::Visitor(module.global_indices.id_map), instr.index);
    instrs.emplace_back<WasmVM::Instr::Global_get>(index);
}

#define numeric_const(Ins, Kwd, Ty) \
template<> std::optional<Parse::Instr::Ins> \
    Parse::Instr::Ins::get(TokenIter& begin, TokenHolder& holder) \
{ \
    std::list<TokenType>::iterator it = begin; \
    auto syntax = Parse::Rule<Token::Keyword<Kwd>, Token::Number>::get(it, holder); \
    if(syntax){ \
        begin = it; \
        Parse::Instr::Ins instr; \
        instr.value = std::get<1>(syntax.value()).unpack<Ty>(); \
        return instr; \
    } \
    return std::nullopt; \
} \
void InstrVisitor::ConstSema::operator()(Parse::Instr::Ins& instr){ \
    instrs.emplace_back(instr); \
} \
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Ins>(Parse::Instr::Ins& instr){ \
    instrs.emplace_back(instr); \
}

numeric_const(I32_const, "i32.const", i32_t)
numeric_const(I64_const, "i64.const", i64_t)
numeric_const(F32_const, "f32.const", f32_t)
numeric_const(F64_const, "f64.const", f64_t)

void InstrVisitor::ConstSyntax::operator()(WasmVM::Syntax::ConstInstr& plain){
    std::visit(overloaded {
        [&](auto&& instr){
            body.emplace_back(instr);
        }
    }, plain);
}