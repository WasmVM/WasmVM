// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

#include <exception.hpp>

using namespace WasmVM;

void InstrVisitor::operator()(Parse::Instr::Call& node){
    index_t index = std::visit(Parse::Index::Visitor(module.funcid_map), node.index);
    func.body.emplace_back<WasmVM::Instr::Call>(index);
}