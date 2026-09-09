// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include <Util.hpp>
#include <sstream>
#include <set>
#include <exception.hpp>

#include "validate.hpp"

using namespace WasmVM;
using namespace Validate;

// The type section's own rules: a subtype names at most one supertype, which
// must be defined before it (also inside a rec group), must not be final, must
// be the same kind of composite type, and must actually be a supertype -- the
// declaration has to be honest about the shapes.
static std::optional<Exception::Exception> validate_types(const WasmModule& module){
    for(index_t idx = 0; idx < module.types.size(); ++idx){
        const SubType& type = module.types[idx];
        std::stringstream ss;
        ss << "type[" << idx << "]: ";
        if(type.supers.size() > 1){
            ss << "a type has at most one supertype";
            return Exception::Exception(ss.str());
        }
        for(index_t super : type.supers){
            if(super >= idx){
                ss << "supertype " << super << " must be defined before the type";
                return Exception::Exception(ss.str());
            }
            if(module.types[super].final){
                ss << "supertype " << super << " is final";
                return Exception::Exception(ss.str());
            }
            if(module.types[super].comp.index() != type.comp.index()){
                ss << "supertype " << super << " is a different kind of composite type";
                return Exception::Exception(ss.str());
            }
            if(!comp_refines(idx, super, module.types)){
                ss << "type does not refine its supertype " << super;
                return Exception::Exception(ss.str());
            }
        }
    }
    for(const RecGroup& rec : module.recs){
        if((size_t)rec.first + rec.count > module.types.size()){
            return Exception::Exception("rec group out of range of the type section");
        }
    }
    return std::nullopt;
}

std::optional<Exception::Exception> WasmVM::module_validate(const WasmModule& module){
    try {
        if(std::optional<Exception::Exception> error = validate_types(module)){
            return error;
        }
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
        if(module.start){
            if(module.start.value() >= validator.context.funcs.size()){
                return Exception::Exception("start function not found");
            }
            const FuncType& startfunc = validator.context.funcs[module.start.value()];
            if(!startfunc.params.empty() || !startfunc.results.empty()){
                return Exception::Exception("start function should be void and no parameter");
            }
        }
        // exports
        idx = 0;
        std::set<std::string> export_names;
        for(const WasmExport& export_ : module.exports){
            try {
                validator(export_);
                if(export_names.contains(export_.name)){
                    throw Exception::Exception("export name should be unique in module scope");
                }
                export_names.insert(export_.name);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "export[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
        // imports
        idx = 0;
        for(const WasmImport& import : module.imports){
            try {
                validator(import);
            } catch (Exception::Exception e){
                std::stringstream ss;
                ss << "import[" << idx << "]: " << e.what();
                return Exception::Exception(ss.str());
            }
            idx += 1;
        }
    } catch (Exception::Exception e){
        return e;
    }
    return std::nullopt;
}

// A type index that has to name a function type: funcs, tags and their imports.
static void require_func(const std::vector<SubType>& types, index_t index, const std::string& where){
    if(!types[index].is_func()){
        std::stringstream ss;
        ss << where << ": type index " << index << " is not a function type";
        throw Exception::Exception(ss.str());
    }
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
                require_func(module.types, index, "import[" + std::to_string(idx) + "]");
                context.funcs.emplace_back(module.types[index].func());
            },
            [&](const TagType& tag){
                if(tag.typeidx >= module.types.size()){
                    std::stringstream ss;
                    ss << "import[" << idx << "]: type index " << tag.typeidx << " not exist";
                    throw Exception::Exception(ss.str());
                }
                require_func(module.types, tag.typeidx, "import[" + std::to_string(idx) + "]");
                if(!module.types[tag.typeidx].func().results.empty()){
                    std::stringstream ss;
                    ss << "import[" << idx << "]: tag type should have no result";
                    throw Exception::Exception(ss.str());
                }
                context.tags.emplace_back(module.types[tag.typeidx].func());
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
        require_func(module.types, func.typeidx, "func [" + std::to_string(idx) + "]");
        context.funcs.emplace_back(module.types[func.typeidx].func());
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
    // tags
    idx = 0;
    for(index_t typeidx : module.tags){
        if(typeidx >= module.types.size()){
            std::stringstream ss;
            ss << "tag [" << idx << "]: type index " << typeidx << " not exist";
            throw Exception::Exception(ss.str());
        }
        require_func(module.types, typeidx, "tag [" + std::to_string(idx) + "]");
        if(!module.types[typeidx].func().results.empty()){
            std::stringstream ss;
            ss << "tag [" << idx << "]: tag type should have no result";
            throw Exception::Exception(ss.str());
        }
        context.tags.emplace_back(module.types[typeidx].func());
        idx += 1;
    }
}
