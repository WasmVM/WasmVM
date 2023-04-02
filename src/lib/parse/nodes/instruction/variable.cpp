// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "../visitor.hpp"

using namespace WasmVM;

template<> void InstrVisitor::Sema::operator()<Parse::Instr::Local_get>(Parse::Instr::Local_get& node){
    index_t index = std::visit(Parse::Index::Visitor(localid_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Local_get>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Local_set>(Parse::Instr::Local_set& node){
    index_t index = std::visit(Parse::Index::Visitor(localid_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Local_set>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Local_tee>(Parse::Instr::Local_tee& node){
    index_t index = std::visit(Parse::Index::Visitor(localid_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Local_tee>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Global_get>(Parse::Instr::Global_get& node){
    index_t index = std::visit(Parse::Index::Visitor(module.global_indices.id_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Global_get>(index);
}
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Global_set>(Parse::Instr::Global_set& node){
    index_t index = std::visit(Parse::Index::Visitor(module.global_indices.id_map), node.index);
    instrs.emplace_back<WasmVM::Instr::Global_set>(index);
}
