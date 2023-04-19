// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Global& node){
    index_t globalidx = global_indices.records.size();
    // export
    for(std::string name : node.exports){
        module.exports.emplace_back(name, WasmExport::DescType::global, globalidx);
    }
    // import
    if(!node.import.first.empty() || !node.import.second.empty()){
        WasmImport& import = module.imports.emplace_back();
        if(!node.id.empty()){
            if(global_indices.id_map.contains(node.id)){
                throw Exception::duplicated_identifier(node.location, std::string(" : global ") + node.id);
            }
            global_indices.id_map[node.id] = globalidx;
        }
        import.module = node.import.first;
        import.name = node.import.second;
        import.desc.emplace<WasmVM::GlobalType>(node.type);
        global_indices.records.emplace_back(IndexSpace::Type::Import);
        return;
    }
    // id
    if(!node.id.empty()){
        if(global_indices.id_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : global ") + node.id);
        }
        global_indices.id_map[node.id] = globalidx;
    }
    global_indices.records.emplace_back(IndexSpace::Type::Normal);
    // global
    WasmGlobal& global = module.globals.emplace_back();
    global.type = node.type;
    std::vector<WasmVM::ConstInstr> instrs;
    std::visit(InstrVisitor::ConstSema(*this, instrs), node.init.value());
    global.init = instrs.front();
}

std::optional<Parse::Global> Parse::Global::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using importrule = Parse::Rule<
        Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, Token::ParenR,
        Parse::GlobalType
    >;
    using globalrule = Parse::Rule<Parse::GlobalType, Syntax::ConstInstr>;
    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"global">, Parse::Optional<Token::Id>,
        Parse::Repeat<Parse::Rule<Token::ParenL, Token::Keyword<"export">, Token::String, Token::ParenR>>,
        Parse::OneOf<importrule, globalrule>,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Global global;
        auto rule = syntax.value();
        // location
        global.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        global.id = id ? id->value : "";
        // export
        for(auto node : std::get<3>(rule)){
            global.exports.emplace_back(std::get<2>(node).value);
        }
        // import/global
        std::visit(overloaded {
            // import
            [&](importrule& node){
                global.import = std::pair<std::string, std::string>(std::get<2>(node).value, std::get<3>(node).value);
                global.type = std::get<5>(node);
            },
            // global
            [&](globalrule& node){
                global.type = std::get<0>(node);
                std::vector<Instr::ConstInstr> instrs;
                InstrVisitor::ConstSyntax visitor(instrs);
                visitor(std::get<1>(node));
                global.init = instrs.front();
            }
        }, std::get<4>(rule));
        begin = it;
        return global;
    }
    return std::nullopt;
}