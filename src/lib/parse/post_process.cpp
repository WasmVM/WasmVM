// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ParseContext.hpp"

#include <unordered_map>
#include <queue>

using namespace WasmVM;
using namespace WasmVM::Parse;

std::unordered_map<index_t, index_t> ParseContext::reorder_map(IndexSpace& space) {
    std::unordered_map<index_t, index_t> result;
    std::queue<index_t> indices;
    for (index_t i = 0; i < space.records.size(); ++i) {
        if (space.records[i] == IndexSpace::Type::Import) {
            result[i] = result.size();
        } else {
            indices.push(i);
        }
    }
    while (!indices.empty()) {
        result[indices.front()] = result.size();
        indices.pop();
    }
    return result;
}

void ParseContext::post_process() {
    std::unordered_map<index_t, index_t> typemap;
    {
        // Deduplicate types
        std::unordered_map<FuncType, index_t> funcmap;
        std::vector<FuncType> functypes;
        for (index_t i = 0; i < module_.types.size(); ++i) {
            if (funcmap.contains(module_.types[i])) {
                typemap[i] = funcmap[module_.types[i]];
            } else {
                funcmap[module_.types[i]] = functypes.size();
                typemap[i] = functypes.size();
                functypes.emplace_back(module_.types[i]);
            }
        }
        module_.types = functypes;
    }
    std::unordered_map<index_t, index_t> funcmap    = reorder_map(func_map_);
    std::unordered_map<index_t, index_t> tablemap   = reorder_map(table_map_);
    std::unordered_map<index_t, index_t> memorymap  = reorder_map(mem_map_);
    std::unordered_map<index_t, index_t> globalmap  = reorder_map(global_map_);

    // funcs
    for (WasmFunc& func : module_.funcs) {
        if (typemap.contains(func.typeidx))
            func.typeidx = typemap[func.typeidx];
        for (WasmInstr& instr : func.body) {
            std::visit(overloaded {
                [&](Instr::Block& i)          { if (i.type && typemap.contains(i.type.value())) i.type = typemap[i.type.value()]; },
                [&](Instr::Loop& i)           { if (i.type && typemap.contains(i.type.value())) i.type = typemap[i.type.value()]; },
                [&](Instr::If& i)             { if (i.type && typemap.contains(i.type.value())) i.type = typemap[i.type.value()]; },
                [&](Instr::Call_indirect& i)  { if (typemap.contains(i.typeidx)) i.typeidx = typemap[i.typeidx]; },
                [&](Instr::Ref_func& i)       { if (funcmap.contains(i.index)) i.index = funcmap[i.index]; },
                [&](Instr::Global_get& i)     { if (globalmap.contains(i.index)) i.index = globalmap[i.index]; },
                [&](Instr::Global_set& i)     { if (globalmap.contains(i.index)) i.index = globalmap[i.index]; },
                [&](Instr::Table_get& i)      { if (tablemap.contains(i.index)) i.index = tablemap[i.index]; },
                [&](Instr::Table_set& i)      { if (tablemap.contains(i.index)) i.index = tablemap[i.index]; },
                [&](Instr::Table_size& i)     { if (tablemap.contains(i.index)) i.index = tablemap[i.index]; },
                [&](Instr::Table_grow& i)     { if (tablemap.contains(i.index)) i.index = tablemap[i.index]; },
                [&](Instr::Table_fill& i)     { if (tablemap.contains(i.index)) i.index = tablemap[i.index]; },
                [&](Instr::Table_copy& i)     { if (tablemap.contains(i.dstidx)) i.dstidx = tablemap[i.dstidx]; if (tablemap.contains(i.srcidx)) i.srcidx = tablemap[i.srcidx]; },
                [&](Instr::Table_init& i)     { if (tablemap.contains(i.tableidx)) i.tableidx = tablemap[i.tableidx]; },
                [&](Instr::I32_load& i)       { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load& i)       { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::F32_load& i)       { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::F64_load& i)       { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_load8_s& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_load8_u& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_load16_s& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_load16_u& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load8_s& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load8_u& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load16_s& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load16_u& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load32_s& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_load32_u& i)   { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_store& i)      { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_store& i)      { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::F32_store& i)      { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::F64_store& i)      { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_store8& i)     { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I32_store16& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_store8& i)     { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_store16& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::I64_store32& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::Memory_size& i)    { if (memorymap.contains(i.index)) i.index = memorymap[i.index]; },
                [&](Instr::Memory_grow& i)    { if (memorymap.contains(i.index)) i.index = memorymap[i.index]; },
                [&](Instr::Memory_fill& i)    { if (memorymap.contains(i.index)) i.index = memorymap[i.index]; },
                [&](Instr::Memory_init& i)    { if (memorymap.contains(i.memidx)) i.memidx = memorymap[i.memidx]; },
                [&](Instr::Memory_copy& i)    { if (memorymap.contains(i.dstidx)) i.dstidx = memorymap[i.dstidx]; if (memorymap.contains(i.srcidx)) i.srcidx = memorymap[i.srcidx]; },
                [](auto&) {}
            }, instr);
        }
    }
    // elems
    for (WasmElem& elem : module_.elems) {
        if (elem.mode.tableidx && tablemap.contains(elem.mode.tableidx.value()))
            elem.mode.tableidx = tablemap[elem.mode.tableidx.value()];
        for (ConstInstr& item : elem.elemlist) {
            std::visit(overloaded {
                [&](Instr::Global_get& i) { if (globalmap.contains(i.index)) i.index = globalmap[i.index]; },
                [&](Instr::Ref_func& i)   { if (funcmap.contains(i.index)) i.index = funcmap[i.index]; },
                [&](Instr::I32_const& i)  { if (funcmap.contains(i.value)) i.value = funcmap[i.value]; },
                [&](Instr::I64_const& i)  { if (funcmap.contains(i.value)) i.value = funcmap[i.value]; },
                [&](Instr::F32_const& i)  { if (funcmap.contains((index_t)i.value)) i.value = (i.value - (index_t)i.value) + funcmap[(index_t)i.value]; },
                [&](Instr::F64_const& i)  { if (funcmap.contains((index_t)i.value)) i.value = (i.value - (index_t)i.value) + funcmap[(index_t)i.value]; },
                [](auto&) {}
            }, item);
        }
        if (elem.mode.offset) {
            std::visit(overloaded {
                [&](Instr::Global_get& i) { if (globalmap.contains(i.index)) i.index = globalmap[i.index]; },
                [](auto&) {}
            }, elem.mode.offset.value());
        }
    }
    // datas
    for (WasmData& data : module_.datas) {
        if (data.mode.memidx && memorymap.contains(data.mode.memidx.value()))
            data.mode.memidx = memorymap[data.mode.memidx.value()];
        if (data.mode.offset) {
            std::visit(overloaded {
                [&](Instr::Global_get& i) { if (globalmap.contains(i.index)) i.index = globalmap[i.index]; },
                [](auto&) {}
            }, data.mode.offset.value());
        }
    }
    // start
    if (module_.start && funcmap.contains(module_.start.value()))
        module_.start = funcmap[module_.start.value()];
    // exports
    for (WasmExport& exp : module_.exports) {
        switch (exp.desc) {
            case WasmExport::DescType::func:   if (funcmap.contains(exp.index))   exp.index = funcmap[exp.index];   break;
            case WasmExport::DescType::table:  if (tablemap.contains(exp.index))  exp.index = tablemap[exp.index];  break;
            case WasmExport::DescType::mem:    if (memorymap.contains(exp.index)) exp.index = memorymap[exp.index]; break;
            case WasmExport::DescType::global: if (globalmap.contains(exp.index)) exp.index = globalmap[exp.index]; break;
        }
    }
    // imports
    for (WasmImport& imp : module_.imports) {
        if (std::holds_alternative<index_t>(imp.desc) && typemap.contains(std::get<index_t>(imp.desc)))
            imp.desc = typemap[std::get<index_t>(imp.desc)];
    }
}
