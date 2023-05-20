// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <Util.hpp>
#include <sstream>
#include <exception.hpp>

#include "validate.hpp"

using namespace WasmVM;
using namespace Validate;

std::optional<Exception::Exception> WasmVM::module_validate(const WasmModule& module){
    try {
        Validator validator(module);
        // funcs
        size_t idx = 0;
        for(const WasmFunc& func : module.funcs){
            try {
                validator(func);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "func[" << idx << "]:" << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // tables
        idx = 0;
        for(const TableType& table : module.tables){
            try {
                validator(table);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "table[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // mems
        idx = 0;
        for(const MemType& mem : module.mems){
            try {
                validator(mem);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "memory[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // globals
        idx = 0;
        for(const WasmGlobal& global : module.globals){
            try {
                validator(global, idx);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "global[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // elems
        idx = 0;
        for(const WasmElem& elem : module.elems){
            try {
                validator(elem);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "elem[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // datas
        idx = 0;
        for(const WasmData& data : module.datas){
            try {
                validator(data);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "data[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // start
        // imports
        // exports
    } catch (Exception::Exception e){
        return e;
    }
    return std::nullopt;
}

Validator::Validator(const WasmModule& module) : context(module.types, module.elems, module.datas) {
    // imports
    size_t idx = 0;
    for(const WasmImport& import : module.imports){
        std::visit(overloaded {
            [&](const index_t& index){
                if(index >= module.types.size()){
                    std::stringstream ss;
                    ss << "import[" << idx << "]: type index " << index << " not exist";
                    throw Exception::Exception(ss.str());
                }
                context.funcs.emplace_back(module.types[index]);
            },
            [&](const TableType& table){
                context.tables.emplace_back(table);
            },
            [&](const MemType& mem){
                context.mems.emplace_back(mem);
            },
            [&](const GlobalType& global){
                context.globals.emplace_back(global);
            }
        }, import.desc);
        idx += 1;
    }
    // funcs
    idx = 0;
    for(const WasmFunc& func : module.funcs){
        if(func.typeidx >= module.types.size()){
            std::stringstream ss;
            ss << "func [" << idx << "]: type index " << func.typeidx << " not exist";
            throw Exception::Exception(ss.str());
        }
        context.funcs.emplace_back(module.types[func.typeidx]);
        idx += 1;
    }
    // tables
    for(const TableType& table : module.tables){
        context.tables.emplace_back(table);
    }
    // mems
    for(const MemType& memory : module.mems){
        context.mems.emplace_back(memory);
    }
    // globals
    for(const WasmGlobal& global : module.globals){
        context.globals.emplace_back(global.type);
    }
}