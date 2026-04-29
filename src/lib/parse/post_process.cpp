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
            switch(instr.opcode) {
                case Opcode::Block: case Opcode::Loop: case Opcode::If: {
                    auto& blk = std::get<WasmInstr::BlockType>(instr.imm);
                    if (blk.type && typemap.contains(blk.type.value())) blk.type = typemap[blk.type.value()];
                } break;
                case Opcode::Call_indirect: {
                    auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if (typemap.contains(tw.b)) tw.b = typemap[tw.b]; // b = typeidx
                } break;
                case Opcode::Ref_func: {
                    auto& o = std::get<WasmInstr::OneIdx>(instr.imm);
                    if (funcmap.contains(o.index)) o.index = funcmap[o.index];
                } break;
                case Opcode::Global_get: case Opcode::Global_set: {
                    auto& o = std::get<WasmInstr::OneIdx>(instr.imm);
                    if (globalmap.contains(o.index)) o.index = globalmap[o.index];
                } break;
                case Opcode::Table_get: case Opcode::Table_set:
                case Opcode::Table_size: case Opcode::Table_grow: case Opcode::Table_fill: {
                    auto& o = std::get<WasmInstr::OneIdx>(instr.imm);
                    if (tablemap.contains(o.index)) o.index = tablemap[o.index];
                } break;
                case Opcode::Table_copy: {
                    auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if (tablemap.contains(tw.a)) tw.a = tablemap[tw.a]; // a = dstidx
                    if (tablemap.contains(tw.b)) tw.b = tablemap[tw.b]; // b = srcidx
                } break;
                case Opcode::Table_init: {
                    auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if (tablemap.contains(tw.a)) tw.a = tablemap[tw.a]; // a = tableidx
                } break;
                case Opcode::I32_load: case Opcode::I64_load:
                case Opcode::F32_load: case Opcode::F64_load:
                case Opcode::I32_load8_s: case Opcode::I32_load8_u:
                case Opcode::I32_load16_s: case Opcode::I32_load16_u:
                case Opcode::I64_load8_s: case Opcode::I64_load8_u:
                case Opcode::I64_load16_s: case Opcode::I64_load16_u:
                case Opcode::I64_load32_s: case Opcode::I64_load32_u:
                case Opcode::I32_store: case Opcode::I64_store:
                case Opcode::F32_store: case Opcode::F64_store:
                case Opcode::I32_store8: case Opcode::I32_store16:
                case Opcode::I64_store8: case Opcode::I64_store16: case Opcode::I64_store32: {
                    auto& m = std::get<WasmInstr::MemArg>(instr.imm);
                    if (memorymap.contains(m.memidx)) m.memidx = memorymap[m.memidx];
                } break;
                case Opcode::Memory_size: case Opcode::Memory_grow: case Opcode::Memory_fill: {
                    auto& o = std::get<WasmInstr::OneIdx>(instr.imm);
                    if (memorymap.contains(o.index)) o.index = memorymap[o.index];
                } break;
                case Opcode::Memory_init: {
                    auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if (memorymap.contains(tw.a)) tw.a = memorymap[tw.a]; // a = memidx
                } break;
                case Opcode::Memory_copy: {
                    auto& tw = std::get<WasmInstr::TwoIdx>(instr.imm);
                    if (memorymap.contains(tw.a)) tw.a = memorymap[tw.a]; // a = dstidx
                    if (memorymap.contains(tw.b)) tw.b = memorymap[tw.b]; // b = srcidx
                } break;
                default: break;
            }
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
