// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <exception.hpp>
#include <Util.hpp>

using namespace WasmVM;

ModuleInst WasmVM::module_instanciate(Store& store, const WasmModule& module, std::vector<ExternVal> externvals){
    ModuleInst moduleInst;
    // Check external value
    if(externvals.size() != module.imports.size()){
        throw Exception::Exception("external value count not match import count");
    }
    // Insert external values
    for(size_t i = 0; i < externvals.size(); ++i){
        ExternVal& externval = externvals[i];
        const WasmImport& import = module.imports[i];
        switch(externval.type){
            case ExternVal::Func :
                if(!std::holds_alternative<index_t>(import.desc)){
                    throw Exception::Exception("import type not match external value");
                }
                if(externval.addr >= store.funcs.size()){
                    throw Exception::Exception("invalid external function address");
                }
                moduleInst.funcaddrs.emplace_back(externval.addr);
            break;
            case ExternVal::Table :
                if(!std::holds_alternative<TableType>(import.desc)){
                    throw Exception::Exception("import type not match external value");
                }
                if(externval.addr >= store.tables.size()){
                    throw Exception::Exception("invalid external table address");
                }
                moduleInst.tableaddrs.emplace_back(externval.addr);
            break;
            case ExternVal::Mem :
                if(!std::holds_alternative<MemType>(import.desc)){
                    throw Exception::Exception("import type not match external value");
                }
                if(externval.addr >= store.mems.size()){
                    throw Exception::Exception("invalid external memory address");
                }
                moduleInst.memaddrs.emplace_back(externval.addr);
            break;
            case ExternVal::Global :
                if(!std::holds_alternative<GlobalType>(import.desc)){
                    throw Exception::Exception("import type not match external value");
                }
                if(externval.addr >= store.globals.size()){
                    throw Exception::Exception("invalid external global address");
                }
                moduleInst.globaladdrs.emplace_back(externval.addr);
            break;
        }
    }
    // Allocate types
    moduleInst.types = module.types;
    // Allocate funcs
    for(size_t idx = 0; idx < module.funcs.size(); ++idx){
        index_t address = store.funcs.size();
        FuncInst funcinst = store.funcs.emplace_back();
        moduleInst.funcaddrs.emplace_back(address);
        FuncInst::Body& body = funcinst.body.emplace<FuncInst::Body>(moduleInst);
        body.func = module.funcs[idx];
        funcinst.type = moduleInst.types[body.func.typeidx];
    }
    // Allocate tables
    for(size_t idx = 0; idx < module.tables.size(); ++idx){
        index_t address = store.tables.size();
        TableInst tableinst = store.tables.emplace_back();
        moduleInst.tableaddrs.emplace_back(address);
        tableinst.type = module.tables[idx];
        TableInst::Ref initval;
        switch(tableinst.type.reftype){
            case RefType::funcref :
                initval.emplace<funcref_t>(std::nullopt);
            break;
            case RefType::externref :
                initval.emplace<externref_t>(nullptr);
            break;
        }
        tableinst.elems.resize(tableinst.type.limits.min, initval);
    }
    // Allocate mems
    for(size_t idx = 0; idx < module.mems.size(); ++idx){
        index_t address = store.mems.size();
        MemInst meminst = store.mems.emplace_back();
        moduleInst.memaddrs.emplace_back(address);
        meminst.type = module.mems[idx];
        meminst.data.resize(meminst.type.min * page_size);
    }
    // Allocate globals
    for(size_t idx = 0; idx < module.globals.size(); ++idx){
        index_t address = store.globals.size();
        GlobalInst& globalinst = store.globals.emplace_back();
        moduleInst.globaladdrs.emplace_back(address);
        const WasmGlobal& global = module.globals[idx];
        globalinst.type = global.type.type;
        std::visit(overloaded {
            [&](Instr::I32_const instr){
                globalinst.value.emplace<i32_t>(instr.value);
            },
            [&](Instr::I64_const instr){
                globalinst.value.emplace<i64_t>(instr.value);
            },
            [&](Instr::F32_const instr){
                globalinst.value.emplace<f32_t>(instr.value);
            },
            [&](Instr::F64_const instr){
                globalinst.value.emplace<f64_t>(instr.value);
            },
            [&](Instr::Ref_null instr){
                switch(globalinst.type){
                    case ValueType::funcref :
                        globalinst.value.emplace<funcref_t>(std::nullopt);
                    break;
                    case ValueType::externref :
                        globalinst.value.emplace<externref_t>(nullptr);
                    break;
                    default:
                        throw Exception::Exception("invalid global init expression");
                    break;
                }
            },
            [&](Instr::Ref_func instr){
                globalinst.value.emplace<funcref_t>(instr.index);
            },
            [&](Instr::Global_get instr){
                index_t globaladdr = moduleInst.globaladdrs[instr.index];
                if(globaladdr >= store.globals.size()){
                    throw Exception::Exception("invalid global address in const global.get");
                }
                const GlobalInst& got = store.globals[globaladdr];
                if(got.type != globalinst.type){
                    throw Exception::Exception("global type not match in const global.get");
                }
                globalinst.value = got.value;
            }
        }, global.init);
    }
    return moduleInst;
}