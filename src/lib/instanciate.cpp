// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <exception.hpp>
#include <Util.hpp>
#include <instances/Stack.hpp>

using namespace WasmVM;

static void init_table(std::vector<Ref>::iterator it, const WasmElem& elem, ModuleInst& moduleInst, Store& store){
    for(ConstInstr instr : elem.elemlist){
        std::visit(overloaded {
            [&](Instr::Ref_null& ins){
                switch(elem.type){
                    case RefType::funcref :
                        it->emplace<funcref_t>(std::nullopt);
                    break;
                    case RefType::externref :
                        it->emplace<externref_t>(nullptr);
                    break;
                }
            },
            [&](Instr::Ref_func& ins){
                index_t funcaddr = moduleInst.funcaddrs[ins.index];
                if(funcaddr >= store.funcs.size()){
                    throw Exception::Exception("invalid function address in ref.func");
                }
                it->emplace<funcref_t>(funcaddr);
            },
            [&](Instr::Global_get& ins){
                index_t globaladdr = moduleInst.globaladdrs[ins.index];
                if(globaladdr >= store.globals.size()){
                    throw Exception::Exception("invalid global address in const global.get");
                }
                const GlobalInst& got = store.globals[globaladdr];
                if(static_cast<ValueType>(elem.type) != got.type){
                    throw Exception::Exception("global type not match in const global.get");
                }
                switch(elem.type){
                    case RefType::funcref :
                        it->emplace<funcref_t>(std::get<funcref_t>(got.value));
                    break;
                    case RefType::externref :
                        it->emplace<externref_t>(std::get<externref_t>(got.value));
                    break;
                }
            },
            [](auto&){
                throw Exception::Exception("invalid item in element segment");
            }
        }, instr);
        it = std::next(it);
    }
}

static offset_t eval_offset(const std::optional<ConstInstr>& instr, ModuleInst& moduleInst, Store& store){
    if(instr.has_value()){
        return std::visit<offset_t>(overloaded {
            [&](const Instr::Global_get& ins){
                index_t globaladdr = moduleInst.globaladdrs[ins.index];
                if(globaladdr >= store.globals.size()){
                    throw Exception::Exception("invalid global address in const global.get");
                }
                const GlobalInst& got = store.globals[globaladdr];
                return std::visit<offset_t>(overloaded {
                    [&](i32_t val){
                        return (offset_t)val;
                    },
                    [&](i64_t val){
                        return (offset_t)val;
                    },
                    [](auto){
                        throw Exception::Exception("invalid element offset in global.get");
                        return 0;
                    }
                }, got.value);
            },
            [&](const Instr::I32_const& ins){
                return (offset_t)ins.value;
            },
            [&](const Instr::I64_const& ins){
                return (offset_t)ins.value;
            },
            [](auto){
                throw Exception::Exception("invalid element offset expression");
                return 0;
            }
        }, instr.value());
    }else{
        return 0;
    }
}

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
    // Types
    moduleInst.types = module.types;
    // Funcs
    for(size_t idx = 0; idx < module.funcs.size(); ++idx){
        index_t address = store.funcs.size();
        FuncInst& funcinst = store.funcs.emplace_back(moduleInst);
        funcinst.body = module.funcs[idx];
        funcinst.type = moduleInst.types[module.funcs[idx].typeidx];
        moduleInst.funcaddrs.emplace_back(address);
    }
    // Tables
    for(size_t idx = 0; idx < module.tables.size(); ++idx){
        index_t address = store.tables.size();
        TableInst& tableinst = store.tables.emplace_back();
        moduleInst.tableaddrs.emplace_back(address);
        tableinst.type = module.tables[idx];
        Ref initval;
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
    // Memories
    for(size_t idx = 0; idx < module.mems.size(); ++idx){
        index_t address = store.mems.size();
        MemInst meminst = store.mems.emplace_back();
        moduleInst.memaddrs.emplace_back(address);
        meminst.type = module.mems[idx];
        meminst.data.resize(meminst.type.min * page_size);
    }
    // Globals
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
    // Exports
    for(WasmExport export_ : module.exports){
        ExportInst& exportinst = moduleInst.exports.emplace_back();
        exportinst.name = export_.name;
        switch(export_.desc){
            case WasmExport::DescType::func :
                if(moduleInst.funcaddrs[export_.index] >= store.funcs.size()){
                    throw Exception::Exception("exported function not exist in store");
                }
                exportinst.value.type = ExternVal::Func;
                exportinst.value.addr = moduleInst.funcaddrs[export_.index];
            break;
            case WasmExport::DescType::table :
                if(moduleInst.tableaddrs[export_.index] >= store.tables.size()){
                    throw Exception::Exception("exported table not exist in store");
                }
                exportinst.value.type = ExternVal::Table;
                exportinst.value.addr = moduleInst.tableaddrs[export_.index];
            break;
            case WasmExport::DescType::mem :
                if(moduleInst.memaddrs[export_.index] >= store.mems.size()){
                    throw Exception::Exception("exported memory not exist in store");
                }
                exportinst.value.type = ExternVal::Mem;
                exportinst.value.addr = moduleInst.memaddrs[export_.index];
            break;
            case WasmExport::DescType::global :
                if(moduleInst.globaladdrs[export_.index] >= store.globals.size()){
                    throw Exception::Exception("exported global not exist in store");
                }
                exportinst.value.type = ExternVal::Global;
                exportinst.value.addr = moduleInst.globaladdrs[export_.index];
            break;
        }
    }
    // Elems
    for(size_t idx = 0; idx < module.elems.size(); ++idx){
        const WasmElem& elem = module.elems[idx];
        switch(elem.mode.type){
            case WasmElem::ElemMode::Mode::declarative :
            break;
            case WasmElem::ElemMode::Mode::passive : {
                index_t address = store.elems.size();
                ElemInst& eleminst = store.elems.emplace_back();
                moduleInst.elemaddrs.emplace_back(address);
                eleminst.type = elem.type;
                eleminst.elem.resize(elem.elemlist.size());
                init_table(eleminst.elem.begin(), elem, moduleInst, store);
            }break;
            case WasmElem::ElemMode::Mode::active :
                index_t tableidx = elem.mode.tableidx.value_or(0);
                if(tableidx >= moduleInst.tableaddrs.size()){
                    throw Exception::Exception("table not exist in module");
                }
                index_t tableaddr = moduleInst.tableaddrs[tableidx];
                if(tableaddr >= store.tables.size()){
                    throw Exception::Exception("table not found in store");
                }
                TableInst& table = store.tables[tableaddr];
                offset_t offset = eval_offset(elem.mode.offset, moduleInst, store);
                if((offset + elem.elemlist.size()) > table.elems.size()){
                    throw Exception::Exception("insufficient size while initialize table");
                }
                if(table.type.reftype != elem.type){
                    throw Exception::Exception("elem type not match table type");
                }
                init_table(table.elems.begin() + offset, elem, moduleInst, store);
            break;
        }
    }
    // Datas
    for(size_t idx = 0; idx < module.datas.size(); ++idx){
        const WasmData& data = module.datas[idx];
        switch(data.mode.type){
            case WasmData::DataMode::Mode::passive :
                moduleInst.dataaddrs.emplace_back(store.datas.size());
                store.datas.emplace_back(data.init);
            break;
            case WasmData::DataMode::Mode::active :
                index_t memidx = data.mode.memidx.value_or(0);
                if(memidx >= moduleInst.memaddrs.size()){
                    throw Exception::Exception("memory not exist in module");
                }
                index_t memaddr = moduleInst.memaddrs[memidx];
                if(memaddr >= store.tables.size()){
                    throw Exception::Exception("memory not found in store");
                }
                MemInst& memory = store.mems[memaddr];
                offset_t offset = eval_offset(data.mode.offset, moduleInst, store);
                if((offset + data.init.size()) > memory.data.size()){
                    throw Exception::Exception("insufficient size while initialize memory");
                }
                std::copy(data.init.begin(), data.init.end(), memory.data.begin() + offset);
            break;
        }
    }
    // Start
    if(module.start){
        index_t start_index = module.start.value();
        if(start_index >= moduleInst.funcaddrs.size()){
            throw Exception::Exception("start function not exist in module instance");
        }
        index_t start_addr = moduleInst.funcaddrs[start_index];
        if(start_addr >= store.funcs.size()){
            throw Exception::Exception("start function not exist in store");
        }
        Stack stack(store);
        stack.invoke(start_addr, {});
        stack.run();
    }
    return moduleInst;
}