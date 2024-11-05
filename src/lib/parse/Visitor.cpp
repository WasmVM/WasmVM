// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>
#include <tuple>
#include <utility>

using namespace WasmVM;

static std::pair<size_t, size_t> getLocation(antlr4::tree::TerminalNode* node){
    auto symbol = node->getSymbol();
    return {symbol->getLine(), symbol->getCharPositionInLine()};
}

WasmModule Visitor::visit(WatParser::ModuleContext *ctx){
    visitModule(ctx);
    post_process();
    return module;
}

std::any Visitor::visitModule(WatParser::ModuleContext *ctx){
    visitChildren(ctx);
    // types
    std::transform(types.begin(), types.end(), std::back_inserter(module.types), [](auto& type_pair){
        return type_pair.first;
    });
    return module;
}

std::any Visitor::visitModulefield(WatParser::ModulefieldContext *ctx){
    return visitChildren(ctx);
}

std::any Visitor::visitTypesection(WatParser::TypesectionContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(type_map.contains(id)){
            throw Exception::Parse("duplicated type id '" + id + "'", getLocation(ctx->Id()));
        }
        type_map[id] = types.size();
    }
    std::pair<FuncType, std::map<std::string, index_t>> functype = std::any_cast<std::pair<FuncType, std::map<std::string, index_t>>>(visitFunctype(ctx->functype()));
    return types.emplace_back(functype);
}

std::any Visitor::visitValtype(WatParser::ValtypeContext *ctx){
    if(ctx->NumType()){
        std::string numtype = ctx->NumType()->getText();
        if(numtype == "i32"){
            return ValueType(ValueType::i32);
        }else if(numtype == "i64"){
            return ValueType(ValueType::i64);
        }else if(numtype == "f32"){
            return ValueType(ValueType::f32);
        }else{
            return ValueType(ValueType::f64);
        }
    }else{
        std::string reftype = ctx->RefType()->getText();
        if(reftype == "funcref"){
            return ValueType(ValueType::funcref);
        }else{
            return ValueType(ValueType::externref);
        }
    }
}

std::any Visitor::visitFunctype(WatParser::FunctypeContext *ctx){
    std::pair<FuncType, std::map<std::string, index_t>> functype;
    // Param
    for(auto param_tree : ctx->param()){
        std::variant<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, std::vector<ValueType>> param = std::any_cast<std::variant<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, std::vector<ValueType>>>(visitParam(param_tree));
        std::visit(overloaded {
            [&](std::tuple<std::string, ValueType, std::pair<size_t, size_t>>& param_pair){
                if(functype.second.contains(std::get<0>(param_pair))){
                    throw Exception::Parse("duplicated param id '" + std::get<0>(param_pair) + "'", std::get<2>(param_pair));
                }
                functype.second[std::get<0>(param_pair)] = functype.first.params.size();
                functype.first.params.emplace_back(std::get<1>(param_pair));
            },
            [&](std::vector<ValueType>& params){
                functype.first.params.insert(functype.first.params.end(), params.begin(), params.end());
            }
        }, param);
    }
    // Result
    for(auto result_tree : ctx->result()){
        auto results = std::any_cast<std::vector<ValueType>>(visitResult(result_tree));
        functype.first.results.insert(functype.first.results.end(), results.begin(), results.end());
    }
    
    return functype;
}
std::any Visitor::visitParam(WatParser::ParamContext *ctx){
    std::variant<
        std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, 
        std::vector<ValueType>
    > param;
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        ValueType valuetype = std::any_cast<ValueType>(visitValtype(ctx->valtype(0)));
        param = std::tuple<std::string, ValueType, std::pair<size_t, size_t>> {id, valuetype, getLocation(ctx->Id())};
    }else{
        std::vector<ValueType>& valuetypes = param.emplace<std::vector<ValueType>>();
        for(auto valuetype : ctx->valtype()){
            valuetypes.emplace_back(std::any_cast<ValueType>(visitValtype(valuetype)));
        }
    }
    return param;
}
std::any Visitor::visitResult(WatParser::ResultContext *ctx){
    std::vector<ValueType> results;
    for(auto valuetype : ctx->valtype()){
        results.emplace_back(std::any_cast<ValueType>(visitValtype(valuetype)));
    }
    return results;
}

std::any Visitor::visitFuncsection(WatParser::FuncsectionContext *ctx){
    index_t func_idx = func_map.records.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(func_map.id_map.contains(id)){
            throw Exception::Parse("duplicated func id '" + id + "'", getLocation(ctx->Id()));
        }
        func_map.id_map[id] = func_idx;
    }
    // exports
    for(auto export_ctx : ctx->exportabbr()){
        WasmExport& exp = module.exports.emplace_back();
        exp.name = std::any_cast<std::string>(visitExportabbr(export_ctx));
        exp.index = func_idx;
        exp.desc = WasmExport::DescType::func;
    }
    // body
    if(ctx->importabbr() != nullptr){
        // import
        func_map.records.emplace_back(IndexSpace::Type::Import);
        auto import_pair = std::any_cast<std::pair<std::string, std::string>>(visitImportabbr(ctx->importabbr()));
        WasmImport& import = module.imports.emplace_back();
        import.desc = std::any_cast<index_t>(visitTypeuse(ctx->typeuse()));
        import.module = import_pair.first;
        import.name = import_pair.second;
    }else{
        // normal
        func_map.records.emplace_back(IndexSpace::Type::Normal);
        WasmFunc& func = module.funcs.emplace_back();
        func.typeidx = std::any_cast<index_t>(visitTypeuse(ctx->typeuse()));
        // local
        local_map = types[func.typeidx].second;
        for(auto local_ctx : ctx->local()){
            auto locals = std::any_cast<std::vector<ValueType>>(visitLocal(local_ctx));
            func.locals.insert(func.locals.end(), locals.begin(), locals.end());
        }
        // instr
        for(auto instr_ctx : ctx->instr()){
            auto instrs = std::any_cast<std::vector<WasmInstr>>(visitInstr(instr_ctx));
            func.body.insert(func.body.end(), instrs.begin(), instrs.end());
        }
        // epilogue
        func.body.emplace_back(Instr::End());
        local_map.clear();
    }
    return func_idx;
}

std::any Visitor::visitTypeuse(WatParser::TypeuseContext *ctx){
    
    index_t typeidx = 0;
    auto params = ctx->param();
    auto results = ctx->result();
    if(ctx->typeidx() == nullptr){
        typeidx = types.size();
        types.emplace_back();
    }else{
        typeidx = std::any_cast<index_t>(visitTypeidx(ctx->typeidx()));
    }
    if(params.empty() && results.empty()){
        return typeidx;
    }
    // create new type
    auto& new_type = types.emplace_back(types[typeidx]);
    typeidx = types.size() - 1;
    // params
    for(auto param_ctx : params){
        auto param = std::any_cast<std::variant<
            std::tuple<std::string, ValueType, std::pair<size_t, size_t>>, 
            std::vector<ValueType>
        >>(visitParam(param_ctx));
        if(std::holds_alternative<std::vector<ValueType>>(param)){
            std::vector<ValueType>& values = std::get<std::vector<ValueType>>(param);
            new_type.first.params.insert(new_type.first.params.end(), values.begin(), values.end());
        }else{
            auto param_tuple = std::get<std::tuple<std::string, ValueType, std::pair<size_t, size_t>>>(param);
            std::string param_id = std::get<0>(param_tuple);
            if(new_type.second.contains(param_id)){
                throw Exception::Parse("duplicated param id '" + param_id + "'", std::get<2>(param_tuple));
            }
            new_type.second[param_id] = new_type.first.params.size();
            new_type.first.params.emplace_back(std::get<1>(param_tuple));
        }
    }
    // results
    for(auto result_ctx : results){
        auto result = std::any_cast<std::vector<ValueType>>(visitResult(result_ctx));
        new_type.first.results.insert(new_type.first.results.end(), result.begin(), result.end());
    }
    return typeidx;
}

std::any Visitor::visitTypeidx(WatParser::TypeidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(type_map.contains(id)){
            return type_map[id];
        }else{
            throw Exception::Parse("type id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitFuncidx(WatParser::FuncidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(func_map.id_map.contains(id)){
            return func_map.id_map[id];
        }else{
            throw Exception::Parse("func id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}
std::any Visitor::visitI32(WatParser::I32Context *ctx){
    return (i32_t) std::stoi(ctx->Integer()->getText());
}
std::any Visitor::visitI64(WatParser::I64Context *ctx){
    return (i64_t) std::stoll(ctx->Integer()->getText());
}
std::any Visitor::visitU32(WatParser::U32Context *ctx){
    return (u32_t) std::stoul(ctx->Unsigned()->getText());
}
std::any Visitor::visitU64(WatParser::U64Context *ctx){
    return (u64_t) std::stoull(ctx->Unsigned()->getText());
}
std::any Visitor::visitF32(WatParser::F32Context *ctx){
    return (f32_t) std::stof(ctx->Float()->getText());
}
std::any Visitor::visitF64(WatParser::F64Context *ctx){
    return (f64_t) std::stod(ctx->Float()->getText());
}


std::any Visitor::visitImportabbr(WatParser::ImportabbrContext *ctx){
    std::string module = ctx->String(0)->getText();
    std::string name = ctx->String(1)->getText();
    return std::pair<std::string, std::string>(
        module.substr(1, module.size() - 2),
        name.substr(1, name.size() - 2)
    );
}

std::any Visitor::visitLocal(WatParser::LocalContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(local_map.contains(id)){
            throw Exception::Parse("duplicated local id '" + id + "'", getLocation(ctx->Id()));
        }
    }
    std::vector<ValueType> values;
    for(auto value_ctx : ctx->valtype()){
        values.emplace_back(std::any_cast<ValueType>(visitValtype(value_ctx)));
    }
    return values;
}

std::any Visitor::visitExportabbr(WatParser::ExportabbrContext *ctx){
    std::string str = ctx->String()->getText();
    return str.substr(1, str.size() - 2);
}

std::any Visitor::visitStartsection(WatParser::StartsectionContext *ctx){
    module.start = std::any_cast<index_t>(visitFuncidx(ctx->funcidx()));
    return module.start;
}

std::any Visitor::visitImportsection(WatParser::ImportsectionContext *ctx){
    auto desc = std::any_cast<std::variant<
        index_t, TableType, MemType, GlobalType
    >>(visitImportdesc(ctx->importdesc()));
    WasmImport& import = module.imports.emplace_back();
    import.module = ctx->String(0)->getText();
    import.module = import.module.substr(1, import.module.size() - 2);
    import.name = ctx->String(1)->getText();
    import.name = import.name.substr(1, import.name.size() - 2);
    import.desc = desc;
    return import;
}

std::any Visitor::visitImportdesc(WatParser::ImportdescContext *ctx){
    std::variant<
        index_t, TableType, MemType, GlobalType
    > desc;
    if(ctx->typeuse() != nullptr){
        desc = std::any_cast<index_t>(visitTypeuse(ctx->typeuse()));
    }else if(ctx->tabletype() != nullptr){
        desc = std::any_cast<TableType>(visitTabletype(ctx->tabletype()));
    }else if(ctx->memtype() != nullptr){
        desc = std::any_cast<MemType>(visitMemtype(ctx->memtype()));
    }else{
        desc = std::any_cast<GlobalType>(visitGlobaltype(ctx->globaltype()));
    }
    return desc;
}

std::any Visitor::visitTabletype(WatParser::TabletypeContext *ctx){
    std::string reftype = ctx->RefType()->getText();
    return TableType {
        .limits = std::any_cast<Limits>(visitLimits(ctx->limits())),
        .reftype = (reftype == "externref") ? RefType::externref : RefType::funcref
    };
}

std::any Visitor::visitMemtype(WatParser::MemtypeContext *ctx){
    return std::any_cast<MemType>(visitLimits(ctx->limits()));
}

std::any Visitor::visitGlobaltype(WatParser::GlobaltypeContext *ctx){
    return GlobalType {
        .mut = (ctx->children.size() > 1) ? GlobalType::variable : GlobalType::constant,
        .type = std::any_cast<ValueType>(visitValtype(ctx->valtype()))
    };
}

std::any Visitor::visitLimits(WatParser::LimitsContext *ctx){
    return Limits {
        .min = std::any_cast<u64_t>(visitU64(ctx->u64(0))),
        .max = (ctx->u64().size() > 1) ? std::optional<offset_t>(std::any_cast<u64_t>(visitU64(ctx->u64(1)))) : std::nullopt
    };
}