// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

#include <algorithm>
#include <tuple>
#include <utility>
#include <cstring>

using namespace WasmVM;

std::pair<size_t, size_t> WasmVM::getLocation(antlr4::tree::TerminalNode* node){
    auto symbol = node->getSymbol();
    return {symbol->getLine(), symbol->getCharPositionInLine()};
}

static std::string unquote(std::string str){
    str = str.substr(1, str.size() - 2);
    return str;
}

static std::string removeUnderscore(std::string str){
    str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
    return str;
}

static std::string toDataString(std::string str){
    std::string datastr;
    str = unquote(str);
    for(auto it = str.begin(); it != str.end(); it = std::next(it)){
        if(*it == '\\'){
            it = std::next(it);
            switch(*it){
                case 't':
                    datastr += '\t';
                break;
                case 'n':
                    datastr += '\n';
                break;
                case 'r':
                    datastr += '\r';
                break;
                case '"':
                    datastr += '"';
                break;
                case '\'':
                    datastr += '\'';
                break;
                case '\\':
                    datastr += '\\';
                break;
                case 'u':{
                    it = std::next(it, 2);
                    std::string valstr;
                    while(*it != '}'){
                        valstr += *it;
                        it = std::next(it);
                    }
                    u32_t val = std::stoul(removeUnderscore(valstr), nullptr, 16);
                    if(val == 0){
                        datastr += '\0';
                    }else{
                        while(val != 0){
                            datastr += val & 0xff;
                            val >>= CHAR_BIT;
                        }
                    }
                }break;
                default:{
                    std::string valstr;
                    datastr += std::stoi(std::string() + *it + *std::next(it), nullptr, 16);
                    it = std::next(it);
                }break;
            }
        }else{
            datastr += *it;
        }
    }
    return datastr;
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
    return (i32_t) std::stoi(removeUnderscore(ctx->Integer()->getText()));
}
std::any Visitor::visitI64(WatParser::I64Context *ctx){
    return (i64_t) std::stoll(removeUnderscore(ctx->Integer()->getText()));
}
std::any Visitor::visitU32(WatParser::U32Context *ctx){
    return (u32_t) std::stoul(removeUnderscore(ctx->Integer()->getText()));
}
std::any Visitor::visitU64(WatParser::U64Context *ctx){
    return (u64_t) std::stoull(removeUnderscore(ctx->Integer()->getText()));
}
std::any Visitor::visitF32(WatParser::F32Context *ctx){
    std::string text = removeUnderscore(ctx->Float()->getText());
    if(text.starts_with("nan:0x")){
        return (f32_t) std::nanf(text.substr(4).c_str());
    }
    return (f32_t) std::stof(text);
}
std::any Visitor::visitF64(WatParser::F64Context *ctx){
    std::string text = removeUnderscore(ctx->Float()->getText());
    if(text.starts_with("nan:0x")){
        return (f32_t) std::nan(text.substr(4).c_str());
    }
    return (f64_t) std::stod(text);
}


std::any Visitor::visitImportabbr(WatParser::ImportabbrContext *ctx){
    return std::pair<std::string, std::string>(
        unquote(ctx->String(0)->getText()),
        unquote(ctx->String(1)->getText())
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
    return unquote(ctx->String()->getText());
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
    import.module = unquote(ctx->String(0)->getText());
    import.name = unquote(ctx->String(1)->getText());
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

std::any Visitor::visitTablesection(WatParser::TablesectionContext *ctx){
    index_t table_idx = table_map.records.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(table_map.id_map.contains(id)){
            throw Exception::Parse("duplicated table id '" + id + "'", getLocation(ctx->Id()));
        }
        table_map.id_map[id] = table_idx;
    }
    // exports
    for(auto export_ctx : ctx->exportabbr()){
        WasmExport& exp = module.exports.emplace_back();
        exp.name = std::any_cast<std::string>(visitExportabbr(export_ctx));
        exp.index = table_idx;
        exp.desc = WasmExport::DescType::table;
    }

    if(ctx->importabbr() != nullptr){
        // import
        table_map.records.emplace_back(IndexSpace::Type::Import);
        auto import_pair = std::any_cast<std::pair<std::string, std::string>>(visitImportabbr(ctx->importabbr()));
        WasmImport& import = module.imports.emplace_back();
        import.desc = std::any_cast<TableType>(visitTabletype(ctx->tabletype()));
        import.module = import_pair.first;
        import.name = import_pair.second;
    }else{
        // normal
        table_map.records.emplace_back(IndexSpace::Type::Normal);
        TableType& table = module.tables.emplace_back();
        if(ctx->tabletype() != nullptr){
            // tabletype
            table = std::any_cast<TableType>(visitTabletype(ctx->tabletype()));
        }else{
            // embedded elem
            table.reftype = (ctx->RefType()->getText() == "funcref") ? RefType::funcref : RefType::externref;
            WasmElem& elem = module.elems.emplace_back();
            elem.type = table.reftype;
            elem.mode = WasmElem::ElemMode {
                .tableidx = table_idx,
                .type = WasmElem::ElemMode::Mode::active,
                .offset = Instr::I32_const()
            };
            if(ctx->funcidx().size() > 0){
                auto funcindices = ctx->funcidx();
                table.limits.min = funcindices.size();
                table.limits.max = funcindices.size();
                std::transform(
                    funcindices.begin(), funcindices.end(),
                    std::back_inserter(elem.elemlist),
                    [&](auto funcidx){
                        return Instr::Ref_func(std::any_cast<index_t>(visitFuncidx(funcidx)));
                    }
                );
            }else{
                auto elemexprs = ctx->elemexpr();
                table.limits.min = elemexprs.size();
                table.limits.max = elemexprs.size();
                std::transform(
                    elemexprs.begin(), elemexprs.end(),
                    std::back_inserter(elem.elemlist),
                    [&](auto elemexpr){
                        return std::any_cast<ConstInstr>(visitElemexpr(elemexpr));
                    }
                );
            }
        }
    }
    return table_idx;
}

std::any Visitor::visitMemorysection(WatParser::MemorysectionContext *ctx){
    index_t mem_idx = mem_map.records.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(mem_map.id_map.contains(id)){
            throw Exception::Parse("duplicated memory id '" + id + "'", getLocation(ctx->Id()));
        }
        mem_map.id_map[id] = mem_idx;
    }
    // exports
    for(auto export_ctx : ctx->exportabbr()){
        WasmExport& exp = module.exports.emplace_back();
        exp.name = std::any_cast<std::string>(visitExportabbr(export_ctx));
        exp.index = mem_idx;
        exp.desc = WasmExport::DescType::mem;
    }

    if(ctx->importabbr() != nullptr){
        // import
        mem_map.records.emplace_back(IndexSpace::Type::Import);
        auto import_pair = std::any_cast<std::pair<std::string, std::string>>(visitImportabbr(ctx->importabbr()));
        WasmImport& import = module.imports.emplace_back();
        import.desc = std::any_cast<MemType>(visitMemtype(ctx->memtype()));
        import.module = import_pair.first;
        import.name = import_pair.second;
    }else{
        // normal
        mem_map.records.emplace_back(IndexSpace::Type::Normal);
        MemType& mem = module.mems.emplace_back();
        if(ctx->memtype() != nullptr){
            // memtype
            mem = std::any_cast<MemType>(visitMemtype(ctx->memtype()));
        }else{
            // embedded data
            WasmData& data = module.datas.emplace_back();
            data.mode = WasmData::DataMode {
                .memidx = mem_idx,
                .type = WasmData::DataMode::Mode::active,
                .offset = Instr::I64_const()
            };
            for(auto datactx : ctx->String()){
                std::string datastr = toDataString(datactx->getText());
                size_t orig_size = data.init.size();
                data.init.resize(orig_size + datastr.size());
                std::memcpy(data.init.data() + orig_size, datastr.data(), datastr.size());
            }
            mem.min = (data.init.size() + page_size - 1) / page_size;
            mem.max = mem.min;
        }
    }
    return mem_idx;
}

std::any Visitor::visitElemexpr(WatParser::ElemexprContext *ctx){
    return visitConstexpr(ctx->constexpr_());
}

std::any Visitor::visitConstexpr(WatParser::ConstexprContext *ctx){
    std::string instr = ctx->children[0]->getText();
    if(instr == "i32.const"){
        return ConstInstr {Instr::I32_const(std::any_cast<i32_t>(visitI32(ctx->i32())))};
    }else if(instr == "i64.const"){
        return ConstInstr {Instr::I64_const(std::any_cast<i64_t>(visitI64(ctx->i64())))};
    }else if(instr == "f32.const"){
        return ConstInstr {Instr::F32_const(std::any_cast<f32_t>(visitF32(ctx->f32())))};
    }else if(instr == "f64.const"){
        return ConstInstr {Instr::F64_const(std::any_cast<f64_t>(visitF64(ctx->f64())))};
    }else if(instr.starts_with("ref.null")){
        return ConstInstr {Instr::Ref_null(instr.ends_with("func") ? RefType::funcref : RefType::externref)};
    }else if(instr == "ref.func"){
        return ConstInstr {Instr::Ref_func(std::any_cast<index_t>(visitFuncidx(ctx->funcidx())))};
    }else{
        return ConstInstr {Instr::Global_get(std::any_cast<index_t>(visitGlobalidx(ctx->globalidx())))};
    }
}

std::any Visitor::visitGlobalidx(WatParser::GlobalidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(global_map.id_map.contains(id)){
            return global_map.id_map[id];
        }else{
            throw Exception::Parse("global id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitGlobalsection(WatParser::GlobalsectionContext *ctx){
    index_t global_idx = global_map.records.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(global_map.id_map.contains(id)){
            throw Exception::Parse("duplicated global id '" + id + "'", getLocation(ctx->Id()));
        }
        global_map.id_map[id] = global_idx;
    }
    // exports
    for(auto export_ctx : ctx->exportabbr()){
        WasmExport& exp = module.exports.emplace_back();
        exp.name = std::any_cast<std::string>(visitExportabbr(export_ctx));
        exp.index = global_idx;
        exp.desc = WasmExport::DescType::global;
    }

    if(ctx->importabbr() != nullptr){
        // import
        global_map.records.emplace_back(IndexSpace::Type::Import);
        auto import_pair = std::any_cast<std::pair<std::string, std::string>>(visitImportabbr(ctx->importabbr()));
        WasmImport& import = module.imports.emplace_back();
        import.desc = std::any_cast<GlobalType>(visitGlobaltype(ctx->globaltype()));
        import.module = import_pair.first;
        import.name = import_pair.second;
    }else{
        // normal
        global_map.records.emplace_back(IndexSpace::Type::Normal);
        WasmGlobal& global = module.globals.emplace_back();
        global.type = std::any_cast<GlobalType>(visitGlobaltype(ctx->globaltype()));
        global.init = std::any_cast<ConstInstr>(visitConstexpr(ctx->constexpr_()));
    }
    return global_idx;
}

std::any Visitor::visitExportdesc(WatParser::ExportdescContext *ctx){
    if(ctx->funcidx() != nullptr){
        return std::pair<WasmExport::DescType, index_t>(
            WasmExport::DescType::func,
            std::any_cast<index_t>(visitFuncidx(ctx->funcidx()))
        );
    }else if(ctx->tableidx() != nullptr){
        return std::pair<WasmExport::DescType, index_t>(
            WasmExport::DescType::table,
            std::any_cast<index_t>(visitTableidx(ctx->tableidx()))
        );
    }else if(ctx->memidx() != nullptr){
        return std::pair<WasmExport::DescType, index_t>(
            WasmExport::DescType::mem,
            std::any_cast<index_t>(visitMemidx(ctx->memidx()))
        );
    }else{
        return std::pair<WasmExport::DescType, index_t>(
            WasmExport::DescType::global,
            std::any_cast<index_t>(visitGlobalidx(ctx->globalidx()))
        );
    }
}

std::any Visitor::visitExportsection(WatParser::ExportsectionContext *ctx){
    WasmExport& export_ = module.exports.emplace_back();
    export_.name = unquote(ctx->String()->getText());
    auto desc = std::any_cast<std::pair<WasmExport::DescType, index_t>>(visitExportdesc(ctx->exportdesc()));
    export_.desc = desc.first;
    export_.index = desc.second;
    return export_;
}

std::any Visitor::visitTableidx(WatParser::TableidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(table_map.id_map.contains(id)){
            return table_map.id_map[id];
        }else{
            throw Exception::Parse("table id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitMemidx(WatParser::MemidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(mem_map.id_map.contains(id)){
            return mem_map.id_map[id];
        }else{
            throw Exception::Parse("memory id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitElemlist(WatParser::ElemlistContext *ctx){
    std::pair<RefType, std::vector<ConstInstr>> elem_pair;
    if(ctx->RefType() == nullptr){
        elem_pair.first = RefType::funcref;
        auto funcindices = ctx->funcidx();
        std::transform(
            funcindices.begin(), funcindices.end(),
            std::back_inserter(elem_pair.second),
            [&](auto funcidx){
                return ConstInstr {Instr::Ref_func(std::any_cast<index_t>(visitFuncidx(funcidx)))};
            }
        );
    }else{
        elem_pair.first = (ctx->RefType()->getText() == "funcref") ? RefType::funcref : RefType::externref;
        auto elemexprs = ctx->elemexpr();
        std::transform(
            elemexprs.begin(), elemexprs.end(),
            std::back_inserter(elem_pair.second),
            [&](auto elemexpr){
                return std::any_cast<ConstInstr>(visitElemexpr(elemexpr));
            }
        );
    }
    return elem_pair;
}

std::any Visitor::visitElemsection(WatParser::ElemsectionContext *ctx){
    index_t elem_idx = module.elems.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(elem_map.contains(id)){
            throw Exception::Parse("duplicated elem id '" + id + "'", getLocation(ctx->Id()));
        }
        elem_map[id] = elem_idx;
    }
    
    WasmElem& elem = module.elems.emplace_back();
    if(ctx->constexpr_() != nullptr){
        // active
        elem.mode.type = WasmElem::ElemMode::Mode::active;
        elem.mode.offset = std::any_cast<ConstInstr>(visitConstexpr(ctx->constexpr_()));
        if(ctx->tableuse() != nullptr){
            elem.mode.tableidx = std::any_cast<index_t>(visitTableuse(ctx->tableuse()));
        }else{
            elem.mode.tableidx = 0;
        }
        if(ctx->funcidx().size() > 0){
            elem.type = RefType::funcref;
            auto funcindices = ctx->funcidx();
            std::transform(
                funcindices.begin(), funcindices.end(),
                std::back_inserter(elem.elemlist),
                [&](auto funcidx){
                    return ConstInstr {Instr::Ref_func(std::any_cast<index_t>(visitFuncidx(funcidx)))};
                }
            );
            return elem;
        }
    }else{
        if(std::any_of(ctx->children.begin(), ctx->children.end(), [](auto child){
            return (child->getTreeType() == antlr4::tree::ParseTreeType::TERMINAL)
                && (child->getText() == "declare");
        })){
            // declarative
            elem.mode.type = WasmElem::ElemMode::Mode::declarative;
        }else{
            // passive
            elem.mode.type = WasmElem::ElemMode::Mode::passive;
        }
    }
    auto elem_pair = std::any_cast<std::pair<RefType, std::vector<ConstInstr>>>(visitElemlist(ctx->elemlist()));
    elem.type = elem_pair.first;
    elem.elemlist = elem_pair.second;
    return elem;
}

std::any Visitor::visitTableuse(WatParser::TableuseContext *ctx){
    return visitTableidx(ctx->tableidx());
}

std::any Visitor::visitDatasection(WatParser::DatasectionContext *ctx){
    index_t data_idx = module.datas.size();
    // id
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(data_map.contains(id)){
            throw Exception::Parse("duplicated data id '" + id + "'", getLocation(ctx->Id()));
        }
        data_map[id] = data_idx;
    }

    WasmData& data = module.datas.emplace_back();
    if(ctx->constexpr_() != nullptr){
        // active
        data.mode.type = WasmData::DataMode::Mode::active;
        data.mode.offset = std::any_cast<ConstInstr>(visitConstexpr(ctx->constexpr_()));
        if(ctx->memuse() != nullptr){
            data.mode.memidx = std::any_cast<index_t>(visitMemuse(ctx->memuse()));
        }else{
            data.mode.memidx = 0;
        }
    }else{
        // passive
        data.mode.type = WasmData::DataMode::Mode::passive;
    }
    for(auto datactx : ctx->String()){
        std::string datastr = toDataString(datactx->getText());
        size_t orig_size = data.init.size();
        data.init.resize(orig_size + datastr.size());
        std::memcpy(data.init.data() + orig_size, datastr.data(), datastr.size());
    }
    return data;
}

std::any Visitor::visitMemuse(WatParser::MemuseContext *ctx){
    return visitMemidx(ctx->memidx());
}

std::any Visitor::visitElemidx(WatParser::ElemidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(elem_map.contains(id)){
            return elem_map[id];
        }else{
            throw Exception::Parse("element id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitDataidx(WatParser::DataidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(data_map.contains(id)){
            return data_map[id];
        }else{
            throw Exception::Parse("data id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitLocalidx(WatParser::LocalidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(local_map.contains(id)){
            return local_map[id];
        }else{
            throw Exception::Parse("local id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitLabelidx(WatParser::LabelidxContext *ctx){
    if(ctx->Id() != nullptr){
        std::string id = ctx->Id()->getText();
        if(label_map.contains(id)){
            return label_map[id] - block_level;
        }else{
            throw Exception::Parse("label id '" + id + "' not found", getLocation(ctx->Id()));
        }
    }else{
        return (index_t)std::any_cast<u32_t>(visitU32(ctx->u32()));
    }
}

std::any Visitor::visitLabel(WatParser::LabelContext *ctx){
    if(ctx->Id() != nullptr){
        return ctx->Id()->getText();
    }else{
        return std::string();
    }
}