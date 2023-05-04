// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visitor.hpp"

using namespace WasmVM;

void ModuleVisitor::operator()(Parse::Memory& node){
    index_t memidx = mem_indices.records.size();
    // export
    for(std::string name : node.exports){
        module.exports.emplace_back(name, WasmExport::DescType::mem, memidx);
    }
    // import
    if(!node.import.first.empty() || !node.import.second.empty()){
        WasmImport& import = module.imports.emplace_back();
        if(!node.id.empty()){
            if(mem_indices.id_map.contains(node.id)){
                throw Exception::duplicated_identifier(node.location, std::string(" : memory ") + node.id);
            }
            mem_indices.id_map[node.id] = memidx;
        }
        import.module = node.import.first;
        import.name = node.import.second;
        import.desc.emplace<WasmVM::MemType>(node.memtype);
        mem_indices.records.emplace_back(IndexSpace::Type::Import);
        return;
    }
    // id
    if(!node.id.empty()){
        if(mem_indices.id_map.contains(node.id)){
            throw Exception::duplicated_identifier(node.location, std::string(" : memory ") + node.id);
        }
        mem_indices.id_map[node.id] = memidx;
    }
    mem_indices.records.emplace_back(IndexSpace::Type::Normal);
    // mem
    module.mems.emplace_back(node.memtype);
    // inits
    if(node.inits.size() > 0){
        WasmData& data = module.datas.emplace_back();
        data.mode.type = WasmData::DataMode::Mode::active;
        data.mode.memidx = memidx;
        data.mode.offset.emplace(WasmVM::Instr::I32_const(0));
        for(Token::String str : node.inits){
            std::transform(str.value.begin() + 1, str.value.end() - 1, std::back_inserter(data.init), [](char c){
            return std::byte(c);
        });
    }
    }
}

std::optional<Parse::Memory> Parse::Memory::get(TokenIter& begin, TokenHolder& holder){
    std::list<TokenType>::iterator it = begin;

    using memrule = Parse::Rule<
        Parse::Optional<Parse::Rule<Token::ParenL, Token::Keyword<"import">, Token::String, Token::String, Token::ParenR>>,
        Parse::MemType
    >;
    using datarule = Parse::Rule<Token::ParenL, Token::Keyword<"data">, Parse::Repeat<Token::String>, Token::ParenR>;

    auto syntax = Parse::Rule<
        Token::ParenL, Token::Keyword<"memory">, Parse::Optional<Token::Id>,
        Parse::Repeat<Parse::Rule<Token::ParenL, Token::Keyword<"export">, Token::String, Token::ParenR>>,
        Parse::OneOf<memrule, datarule>,
        Token::ParenR
    >::get(it, holder);

    if(syntax){
        Parse::Memory memory;
        auto rule = syntax.value();
        // location
        memory.location = std::get<0>(rule).location;
        // id
        auto id = std::get<2>(rule);
        memory.id = id ? id->value : "";
        // export
        for(auto node : std::get<3>(rule)){
            memory.exports.emplace_back(std::get<2>(node).value.substr(1, std::get<2>(node).value.size() - 2));
        }
        // mem/data
        std::visit(overloaded {
            // mem
            [&](memrule& node){
                auto importnode = std::get<0>(node);
                if(importnode){
                    memory.import = std::pair<std::string, std::string>(std::get<2>(importnode.value()).value.substr(1, std::get<2>(importnode.value()).value.size() - 2), std::get<3>(importnode.value()).value.substr(1, std::get<3>(importnode.value()).value.size() - 2));
                }
                memory.memtype = std::get<1>(node);
            },
            // data
            [&](datarule& node){
                // items
                u64_t datasize = 0;
                for(Token::String& data : std::get<2>(node)){
                    memory.inits.emplace_back(data);
                    datasize += data.value.size();
                }
                datasize = (datasize + (page_size - 1)) / page_size; // ceil(size/64Ki)
                memory.memtype.min = datasize;
                memory.memtype.max = datasize;
            }
        }, std::get<4>(rule));

        begin = it;
        return memory;
    }
    return std::nullopt;
}