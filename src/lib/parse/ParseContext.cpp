// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ParseContext.hpp"
#include <WasmVM.hpp>

#include <algorithm>
#include <cstring>
#include <cmath>
#include <tuple>

using namespace WasmVM;
using namespace WasmVM::Parse;

// ── String helpers ────────────────────────────────────────────────────────────

static std::string removeUnderscore(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
    return str;
}

// Convert a raw quoted string token (including surrounding quotes) to bytes.
std::string ParseContext::parse_string_to_bytes(const std::string& raw) {
    // strip surrounding quotes
    std::string str = raw.substr(1, raw.size() - 2);
    std::string datastr;
    for (auto it = str.begin(); it != str.end(); it = std::next(it)) {
        if (*it != '\\') { datastr += *it; continue; }
        it = std::next(it);
        switch (*it) {
            case 't':  datastr += '\t'; break;
            case 'n':  datastr += '\n'; break;
            case 'r':  datastr += '\r'; break;
            case '"':  datastr += '"';  break;
            case '\'': datastr += '\''; break;
            case '\\': datastr += '\\'; break;
            case 'u': {
                it = std::next(it, 2); // skip 'u{'
                std::string valstr;
                while (*it != '}') { valstr += *it; it = std::next(it); }
                u32_t val = std::stoul(removeUnderscore(valstr), nullptr, 16);
                if (val == 0) { datastr += '\0'; }
                else { while (val) { datastr += (char)(val & 0xff); val >>= 8; } }
            } break;
            default: {
                datastr += (char)std::stoi(std::string() + *it + *std::next(it), nullptr, 16);
                it = std::next(it);
            } break;
        }
    }
    return datastr;
}

// ── Constructor / entry point ────────────────────────────────────────────────

ParseContext::ParseContext(Tokenizer& tok) : tok_(tok) {}

WasmModule ParseContext::parse() {
    parse_module();
    // Populate module_.types from internal types_ vector
    std::transform(types_.begin(), types_.end(), std::back_inserter(module_.types),
        [](auto& p) { return p.first; });
    post_process();
    return module_;
}

// ── Module / module fields ────────────────────────────────────────────────────

void ParseContext::parse_module() {
    if (tok_.at_eof()) return;

    // Either '(' 'module' Id? modulefield* ')' or plain modulefield*
    if (tok_.peek().type == TokenType::LParen) {
        // peek ahead to see if it's (module ...) or () empty module
        const Token& peek2 = tok_.peek2();
        if (peek2.type == TokenType::Keyword && peek2.text == "module") {
            tok_.consume(); // '('
            tok_.consume(); // 'module'
            // optional Id
            if (tok_.peek().type == TokenType::Id) tok_.consume();
            // modulefields until ')'
            while (!tok_.at_eof() && tok_.peek().type != TokenType::RParen)
                parse_modulefield();
            tok_.expect(TokenType::RParen, ")");
            return;
        }
        // '()' is treated as empty module
        if (peek2.type == TokenType::RParen) {
            tok_.consume(); // '('
            tok_.consume(); // ')'
            return;
        }
    }
    // plain modulefield*
    while (!tok_.at_eof())
        parse_modulefield();
}

void ParseContext::parse_modulefield() {
    // Each modulefield is '(' keyword ... ')'.
    // Peek inside to see which section.
    if (tok_.peek().type != TokenType::LParen)
        throw Exception::Parse("expected '('", tok_.location());

    const Token& kw = tok_.peek2();
    if (kw.type != TokenType::Keyword)
        throw Exception::Parse("expected section keyword", {kw.line, kw.column});

    const std::string& name = kw.text;
    if      (name == "type")   parse_typesection();
    else if (name == "import") parse_importsection();
    else if (name == "func")   parse_funcsection();
    else if (name == "table")  parse_tablesection();
    else if (name == "memory") parse_memorysection();
    else if (name == "global") parse_globalsection();
    else if (name == "export") parse_exportsection();
    else if (name == "start")  parse_startsection();
    else if (name == "elem")   parse_elemsection();
    else if (name == "data")   parse_datasection();
    else if (name == "tag")    parse_tagsection();
    else throw Exception::Parse("unknown module section '" + name + "'", {kw.line, kw.column});
}

// ── Type section ──────────────────────────────────────────────────────────────

void ParseContext::parse_typesection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("type");
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (type_map_.contains(id))
            throw Exception::Parse("duplicated type id '" + id + "'", tok_.location());
        type_map_[id] = types_.size();
    }
    types_.emplace_back(parse_functype());
    tok_.expect(TokenType::RParen);
}

// ── Valtype / FuncType / type sub-parsers ─────────────────────────────────────

ValueType ParseContext::parse_valtype() {
    Token t = tok_.expect(TokenType::Keyword, "valtype");
    if      (t.text == "i32")      return ValueType(ValueType::i32);
    else if (t.text == "i64")      return ValueType(ValueType::i64);
    else if (t.text == "f32")      return ValueType(ValueType::f32);
    else if (t.text == "f64")      return ValueType(ValueType::f64);
    else if (t.text == "funcref")  return ValueType(ValueType::funcref);
    else if (t.text == "externref") return ValueType(ValueType::externref);
    throw Exception::Parse("unknown valtype '" + t.text + "'", {t.line, t.column});
}

std::pair<FuncType, std::map<std::string, index_t>> ParseContext::parse_functype() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("func");
    std::pair<FuncType, std::map<std::string, index_t>> ft;
    // params
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "param") {
        auto [id, types] = parse_param();
        if (!id.empty()) {
            if (ft.second.contains(id))
                throw Exception::Parse("duplicated param id '" + id + "'", tok_.location());
            ft.second[id] = ft.first.params.size();
        }
        ft.first.params.insert(ft.first.params.end(), types.begin(), types.end());
    }
    // results
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "result") {
        auto results = parse_result();
        ft.first.results.insert(ft.first.results.end(), results.begin(), results.end());
    }
    tok_.expect(TokenType::RParen);
    return ft;
}

std::pair<std::string, std::vector<ValueType>> ParseContext::parse_param() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("param");
    std::pair<std::string, std::vector<ValueType>> result;
    if (tok_.peek().type == TokenType::Id) {
        result.first = tok_.consume().text;
        result.second.push_back(parse_valtype());
    } else {
        while (tok_.peek().type == TokenType::Keyword &&
               (tok_.peek().text == "i32" || tok_.peek().text == "i64" ||
                tok_.peek().text == "f32" || tok_.peek().text == "f64" ||
                tok_.peek().text == "funcref" || tok_.peek().text == "externref")) {
            result.second.push_back(parse_valtype());
        }
    }
    tok_.expect(TokenType::RParen);
    return result;
}

std::vector<ValueType> ParseContext::parse_result() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("result");
    std::vector<ValueType> results;
    while (tok_.peek().type == TokenType::Keyword &&
           (tok_.peek().text == "i32" || tok_.peek().text == "i64" ||
            tok_.peek().text == "f32" || tok_.peek().text == "f64" ||
            tok_.peek().text == "funcref" || tok_.peek().text == "externref")) {
        results.push_back(parse_valtype());
    }
    tok_.expect(TokenType::RParen);
    return results;
}

// ── Limits / MemType / TableType / GlobalType ─────────────────────────────────

Limits ParseContext::parse_limits() {
    Limits lim;
    lim.min = parse_u64();
    if (tok_.peek().type == TokenType::Integer) {
        lim.max = parse_u64();
        if (lim.min > lim.max.value())
            throw Exception::Parse("limit min cannot exceed max", tok_.location());
    }
    return lim;
}

MemType ParseContext::parse_memtype() { return parse_limits(); }

TableType ParseContext::parse_tabletype() {
    TableType tt;
    tt.limits = parse_limits();
    Token ref = tok_.expect(TokenType::Keyword, "RefType");
    tt.reftype = (ref.text == "externref") ? RefType::externref : RefType::funcref;
    return tt;
}

GlobalType ParseContext::parse_globaltype() {
    GlobalType gt;
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "mut") {
        tok_.consume(); tok_.consume(); // '(' 'mut'
        gt.mut = GlobalType::variable;
        gt.type = parse_valtype();
        tok_.expect(TokenType::RParen);
    } else {
        gt.mut = GlobalType::constant;
        gt.type = parse_valtype();
    }
    return gt;
}

// ── Typeuse ───────────────────────────────────────────────────────────────────

bool ParseContext::is_typeuse_start() {
    if (tok_.peek().type != TokenType::LParen) return false;
    const Token& kw = tok_.peek2();
    if (kw.type != TokenType::Keyword) return false;
    return kw.text == "type" || kw.text == "param" || kw.text == "result";
}

index_t ParseContext::parse_typeuse() {
    index_t typeidx = 0;
    bool has_explicit_type = false;

    // Optional '(' 'type' typeidx ')'
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "type") {
        tok_.consume(); tok_.consume(); // '(' 'type'
        typeidx = parse_typeidx();
        tok_.expect(TokenType::RParen);
        has_explicit_type = true;
    }

    if (!has_explicit_type) {
        typeidx = types_.size();
        types_.emplace_back(); // placeholder empty type
    }

    // Check for params/results
    bool has_params_results = false;
    std::vector<std::pair<std::string, std::vector<ValueType>>> params_list;
    std::vector<std::vector<ValueType>> results_list;

    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "param") {
        params_list.push_back(parse_param());
        has_params_results = true;
    }
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "result") {
        results_list.push_back(parse_result());
        has_params_results = true;
    }

    if (!has_params_results) return typeidx;

    // Create a new type by copying the base type and adding params/results.
    // Copy first to avoid reference invalidation when emplace_back reallocates.
    auto base_type = types_[typeidx];
    auto& new_type = types_.emplace_back(std::move(base_type));
    typeidx = (index_t)(types_.size() - 1);

    for (auto& [id, types] : params_list) {
        if (!id.empty()) {
            if (new_type.second.contains(id))
                throw Exception::Parse("duplicated param id '" + id + "'", tok_.location());
            new_type.second[id] = new_type.first.params.size();
        }
        new_type.first.params.insert(new_type.first.params.end(), types.begin(), types.end());
    }
    for (auto& results : results_list) {
        new_type.first.results.insert(new_type.first.results.end(), results.begin(), results.end());
    }
    return typeidx;
}

// ── Numeric parsers ───────────────────────────────────────────────────────────

i32_t ParseContext::parse_i32() {
    return (i32_t)std::stoi(removeUnderscore(tok_.expect(TokenType::Integer).text), nullptr, 0);
}
i64_t ParseContext::parse_i64() {
    return (i64_t)std::stoll(removeUnderscore(tok_.expect(TokenType::Integer).text), nullptr, 0);
}
u32_t ParseContext::parse_u32() {
    return (u32_t)std::stoul(removeUnderscore(tok_.expect(TokenType::Integer).text), nullptr, 0);
}
u64_t ParseContext::parse_u64() {
    return (u64_t)std::stoull(removeUnderscore(tok_.expect(TokenType::Integer).text), nullptr, 0);
}
f32_t ParseContext::parse_f32() {
    std::string text = removeUnderscore(tok_.expect(TokenType::Float).text);
    if (text.find("nan:0x") != std::string::npos) {
        size_t pos = text.find("nan:0x");
        return (f32_t)std::nanf(text.substr(pos + 4).c_str());
    }
    return (f32_t)std::stof(text);
}
f64_t ParseContext::parse_f64() {
    std::string text = removeUnderscore(tok_.expect(TokenType::Float).text);
    if (text.find("nan:0x") != std::string::npos) {
        size_t pos = text.find("nan:0x");
        return (f64_t)std::nan(text.substr(pos + 4).c_str());
    }
    return (f64_t)std::stod(text);
}

// ── Index parsers ─────────────────────────────────────────────────────────────

index_t ParseContext::parse_typeidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!type_map_.contains(t.text))
            throw Exception::Parse("type id '" + t.text + "' not found", {t.line, t.column});
        return type_map_[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_funcidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!func_map_.id_map.contains(t.text))
            throw Exception::Parse("func id '" + t.text + "' not found", {t.line, t.column});
        return func_map_.id_map[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_tableidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!table_map_.id_map.contains(t.text))
            throw Exception::Parse("table id '" + t.text + "' not found", {t.line, t.column});
        return table_map_.id_map[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_memidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!mem_map_.id_map.contains(t.text))
            throw Exception::Parse("memory id '" + t.text + "' not found", {t.line, t.column});
        return mem_map_.id_map[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_globalidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!global_map_.id_map.contains(t.text))
            throw Exception::Parse("global id '" + t.text + "' not found", {t.line, t.column});
        return global_map_.id_map[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_elemidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!elem_map_.contains(t.text))
            throw Exception::Parse("element id '" + t.text + "' not found", {t.line, t.column});
        return elem_map_[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_dataidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!data_map_.contains(t.text))
            throw Exception::Parse("data id '" + t.text + "' not found", {t.line, t.column});
        return data_map_[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_localidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!local_map_.contains(t.text))
            throw Exception::Parse("local id '" + t.text + "' not found", {t.line, t.column});
        return local_map_[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_tagidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!tag_map_.contains(t.text))
            throw Exception::Parse("tag id '" + t.text + "' not found", {t.line, t.column});
        return tag_map_[t.text];
    }
    return (index_t)parse_u32();
}

index_t ParseContext::parse_labelidx() {
    if (tok_.peek().type == TokenType::Id) {
        Token t = tok_.consume();
        if (!label_map_.contains(t.text))
            throw Exception::Parse("label id '" + t.text + "' not found", {t.line, t.column});
        return block_level_ - label_map_[t.text];
    }
    return (index_t)parse_u32();
}

// ── Label ─────────────────────────────────────────────────────────────────────

std::string ParseContext::parse_label() {
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (label_map_.contains(id))
            throw Exception::Parse("duplicated label id '" + id + "'", tok_.location());
        label_map_[id] = block_level_;
        return id;
    }
    return {};
}

// ── Import / export helpers ───────────────────────────────────────────────────

std::pair<std::string, std::string> ParseContext::parse_importabbr() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("import");
    std::string mod  = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    std::string name = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    tok_.expect(TokenType::RParen);
    return {mod, name};
}

std::string ParseContext::parse_exportabbr() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("export");
    std::string name = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    tok_.expect(TokenType::RParen);
    return name;
}

std::pair<WasmExport::DescType, index_t> ParseContext::parse_exportdesc() {
    tok_.expect(TokenType::LParen);
    Token kw = tok_.expect(TokenType::Keyword, "func/table/memory/global");
    index_t idx = 0;
    WasmExport::DescType desc;
    if (kw.text == "func") {
        desc = WasmExport::DescType::func;
        idx = parse_funcidx();
    } else if (kw.text == "table") {
        desc = WasmExport::DescType::table;
        idx = parse_tableidx();
    } else if (kw.text == "memory") {
        desc = WasmExport::DescType::mem;
        idx = parse_memidx();
    } else if (kw.text == "global") {
        desc = WasmExport::DescType::global;
        idx = parse_globalidx();
    } else {
        throw Exception::Parse("unknown export desc '" + kw.text + "'", {kw.line, kw.column});
    }
    tok_.expect(TokenType::RParen);
    return {desc, idx};
}

// ── Local ─────────────────────────────────────────────────────────────────────

std::pair<std::string, std::vector<ValueType>> ParseContext::parse_local() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("local");
    std::pair<std::string, std::vector<ValueType>> result;
    if (tok_.peek().type == TokenType::Id) {
        result.first = tok_.consume().text;
        if (local_map_.contains(result.first))
            throw Exception::Parse("duplicated local id '" + result.first + "'", tok_.location());
        result.second.push_back(parse_valtype());
    } else {
        while (tok_.peek().type == TokenType::Keyword &&
               (tok_.peek().text == "i32" || tok_.peek().text == "i64" ||
                tok_.peek().text == "f32" || tok_.peek().text == "f64" ||
                tok_.peek().text == "funcref" || tok_.peek().text == "externref")) {
            result.second.push_back(parse_valtype());
        }
    }
    tok_.expect(TokenType::RParen);
    return result;
}

// ── Const expression ──────────────────────────────────────────────────────────

ConstInstr ParseContext::parse_constexpr() {
    bool parenthesized = (tok_.peek().type == TokenType::LParen);
    if (parenthesized) tok_.consume(); // '('
    Token kw = tok_.expect(TokenType::Keyword, "const expression");
    ConstInstr result;
    if (kw.text == "i32.const") result = ConstInstr{Instr::I32_const(parse_i32())};
    else if (kw.text == "i64.const") result = ConstInstr{Instr::I64_const(parse_i64())};
    else if (kw.text == "f32.const") result = ConstInstr{Instr::F32_const(parse_f32())};
    else if (kw.text == "f64.const") result = ConstInstr{Instr::F64_const(parse_f64())};
    else if (kw.text == "ref.null") {
        Token ref = tok_.expect(TokenType::Keyword, "func/extern");
        result = ConstInstr{Instr::Ref_null(ref.text == "func" ? RefType::funcref : RefType::externref)};
    }
    else if (kw.text == "ref.func") result = ConstInstr{Instr::Ref_func(parse_funcidx())};
    else if (kw.text == "global.get") result = ConstInstr{Instr::Global_get(parse_globalidx())};
    else throw Exception::Parse("unknown const expression '" + kw.text + "'", {kw.line, kw.column});
    if (parenthesized) tok_.expect(TokenType::RParen);
    return result;
}

ConstInstr ParseContext::parse_elemexpr() {
    tok_.expect(TokenType::LParen);
    // Either '(' 'item' constexpr ')' or '(' constexpr ')'
    if (tok_.peek_keyword("item")) tok_.consume();
    ConstInstr ci = parse_constexpr();
    tok_.expect(TokenType::RParen);
    return ci;
}

std::pair<RefType, std::vector<ConstInstr>> ParseContext::parse_elemlist() {
    std::pair<RefType, std::vector<ConstInstr>> result;
    if (tok_.peek_keyword("func")) {
        tok_.consume(); // 'func'
        result.first = RefType::funcref;
        while (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
            result.second.push_back(ConstInstr{Instr::Ref_func(parse_funcidx())});
    } else {
        // RefType elemexpr*
        Token ref = tok_.expect(TokenType::Keyword, "RefType");
        result.first = (ref.text == "externref") ? RefType::externref : RefType::funcref;
        // consume all '(' 'item'|constexpr-keyword ')' forms
        while (tok_.peek().type == TokenType::LParen) {
            const Token& kw = tok_.peek2();
            if (kw.type != TokenType::Keyword) break;
            if (kw.text != "item" && kw.text != "ref.null" && kw.text != "ref.func" &&
                kw.text != "i32.const" && kw.text != "i64.const" &&
                kw.text != "f32.const" && kw.text != "f64.const" &&
                kw.text != "global.get") break;
            result.second.push_back(parse_elemexpr());
        }
    }
    return result;
}

// ── Section parsers ───────────────────────────────────────────────────────────

void ParseContext::parse_importsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("import");
    std::string mod  = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    std::string name = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    // importdesc
    tok_.expect(TokenType::LParen);
    Token kw = tok_.expect(TokenType::Keyword, "func/table/memory/global");
    WasmImport& import = module_.imports.emplace_back();
    import.module = mod; import.name = name;
    if (kw.text == "func") {
        // optional Id
        if (tok_.peek().type == TokenType::Id) {
            std::string id = tok_.consume().text;
            if (func_map_.id_map.contains(id))
                throw Exception::Parse("duplicated func id '" + id + "'", tok_.location());
            func_map_.id_map[id] = func_map_.records.size();
        }
        func_map_.records.emplace_back(IndexSpace::Type::Import);
        import.desc = parse_typeuse();
    } else if (kw.text == "table") {
        if (tok_.peek().type == TokenType::Id) {
            std::string id = tok_.consume().text;
            if (table_map_.id_map.contains(id))
                throw Exception::Parse("duplicated table id '" + id + "'", tok_.location());
            table_map_.id_map[id] = table_map_.records.size();
        }
        table_map_.records.emplace_back(IndexSpace::Type::Import);
        import.desc = parse_tabletype();
    } else if (kw.text == "memory") {
        if (tok_.peek().type == TokenType::Id) {
            std::string id = tok_.consume().text;
            if (mem_map_.id_map.contains(id))
                throw Exception::Parse("duplicated memory id '" + id + "'", tok_.location());
            mem_map_.id_map[id] = mem_map_.records.size();
        }
        mem_map_.records.emplace_back(IndexSpace::Type::Import);
        import.desc = parse_memtype();
    } else if (kw.text == "global") {
        if (tok_.peek().type == TokenType::Id) {
            std::string id = tok_.consume().text;
            if (global_map_.id_map.contains(id))
                throw Exception::Parse("duplicated global id '" + id + "'", tok_.location());
            global_map_.id_map[id] = global_map_.records.size();
        }
        global_map_.records.emplace_back(IndexSpace::Type::Import);
        import.desc = parse_globaltype();
    } else {
        throw Exception::Parse("unknown import desc '" + kw.text + "'", {kw.line, kw.column});
    }
    tok_.expect(TokenType::RParen); // close importdesc
    tok_.expect(TokenType::RParen); // close import section
}

void ParseContext::parse_funcsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("func");
    index_t func_idx = func_map_.records.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (func_map_.id_map.contains(id))
            throw Exception::Parse("duplicated func id '" + id + "'", tok_.location());
        func_map_.id_map[id] = func_idx;
    }
    // exportabbr*
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "export") {
        WasmExport& exp = module_.exports.emplace_back();
        exp.name = parse_exportabbr();
        exp.index = func_idx;
        exp.desc = WasmExport::DescType::func;
    }
    // import or body
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "import") {
        func_map_.records.emplace_back(IndexSpace::Type::Import);
        auto [imod, iname] = parse_importabbr();
        WasmImport& imp = module_.imports.emplace_back();
        imp.desc = parse_typeuse();
        imp.module = imod; imp.name = iname;
    } else {
        func_map_.records.emplace_back(IndexSpace::Type::Normal);
        WasmFunc& func = module_.funcs.emplace_back();
        func.typeidx = parse_typeuse();
        // set local_map from param ids
        local_map_ = types_[func.typeidx].second;
        // locals
        while (tok_.peek().type == TokenType::LParen &&
               tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "local") {
            auto [id, types] = parse_local();
            if (!id.empty()) {
                local_map_[id] = func.locals.size();
            }
            func.locals.insert(func.locals.end(), types.begin(), types.end());
        }
        // instructions
        while (tok_.peek().type != TokenType::RParen && !tok_.at_eof()) {
            auto instrs = parse_instr();
            func.body.insert(func.body.end(), instrs.begin(), instrs.end());
        }
        func.body.emplace_back(Instr::End());
        local_map_.clear();
        label_map_.clear();
    }
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_tablesection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("table");
    index_t table_idx = table_map_.records.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (table_map_.id_map.contains(id))
            throw Exception::Parse("duplicated table id '" + id + "'", tok_.location());
        table_map_.id_map[id] = table_idx;
    }
    // exportabbr*
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "export") {
        WasmExport& exp = module_.exports.emplace_back();
        exp.name = parse_exportabbr();
        exp.index = table_idx;
        exp.desc = WasmExport::DescType::table;
    }
    // import or inline
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "import") {
        table_map_.records.emplace_back(IndexSpace::Type::Import);
        auto [imod, iname] = parse_importabbr();
        WasmImport& imp = module_.imports.emplace_back();
        imp.desc = parse_tabletype();
        imp.module = imod; imp.name = iname;
    } else {
        table_map_.records.emplace_back(IndexSpace::Type::Normal);
        TableType& table = module_.tables.emplace_back();
        if (tok_.peek().type == TokenType::Integer) {
            // tabletype: limits RefType
            table = parse_tabletype();
        } else if (tok_.peek().type == TokenType::Keyword &&
                   (tok_.peek().text == "funcref" || tok_.peek().text == "externref")) {
            // Inline elem: RefType '(' 'elem' ... ')'
            Token ref = tok_.consume();
            table.reftype = (ref.text == "externref") ? RefType::externref : RefType::funcref;
            tok_.expect(TokenType::LParen);
            tok_.expect_keyword("elem");
            WasmElem& elem = module_.elems.emplace_back();
            elem.type = table.reftype;
            elem.mode = WasmElem::ElemMode{
                .type = WasmElem::ElemMode::Mode::active,
                .tableidx = table_idx,
                .offset = Instr::I32_const()
            };
            // funcidx* or elemexpr*
            if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
                while (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
                    elem.elemlist.push_back(ConstInstr{Instr::Ref_func(parse_funcidx())});
                }
            } else {
                while (tok_.peek().type == TokenType::LParen &&
                       tok_.peek2().type == TokenType::Keyword) {
                    elem.elemlist.push_back(parse_elemexpr());
                }
            }
            offset_t n = (offset_t)elem.elemlist.size();
            table.limits.min = n; table.limits.max = n;
            tok_.expect(TokenType::RParen); // close elem
        }
    }
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_memorysection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("memory");
    index_t mem_idx = mem_map_.records.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (mem_map_.id_map.contains(id))
            throw Exception::Parse("duplicated memory id '" + id + "'", tok_.location());
        mem_map_.id_map[id] = mem_idx;
    }
    // exportabbr*
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "export") {
        WasmExport& exp = module_.exports.emplace_back();
        exp.name = parse_exportabbr();
        exp.index = mem_idx;
        exp.desc = WasmExport::DescType::mem;
    }
    // import or inline
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "import") {
        mem_map_.records.emplace_back(IndexSpace::Type::Import);
        auto [imod, iname] = parse_importabbr();
        WasmImport& imp = module_.imports.emplace_back();
        imp.desc = parse_memtype();
        imp.module = imod; imp.name = iname;
    } else if (tok_.peek().type == TokenType::LParen &&
               tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "data") {
        // Inline data: '(' 'data' String* ')'
        mem_map_.records.emplace_back(IndexSpace::Type::Normal);
        MemType& mem = module_.mems.emplace_back();
        tok_.consume(); tok_.consume(); // '(' 'data'
        WasmData& data = module_.datas.emplace_back();
        data.mode = WasmData::DataMode{
            .type = WasmData::DataMode::Mode::active,
            .memidx = mem_idx,
            .offset = Instr::I64_const()
        };
        while (tok_.peek().type == TokenType::String) {
            std::string bytes = parse_string_to_bytes(tok_.consume().text);
            size_t orig = data.init.size();
            data.init.resize(orig + bytes.size());
            std::memcpy(data.init.data() + orig, bytes.data(), bytes.size());
        }
        tok_.expect(TokenType::RParen); // close data
        offset_t pages = (offset_t)((data.init.size() + page_size - 1) / page_size);
        mem.min = pages; mem.max = pages;
    } else {
        mem_map_.records.emplace_back(IndexSpace::Type::Normal);
        MemType& mem = module_.mems.emplace_back();
        mem = parse_memtype();
    }
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_globalsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("global");
    index_t global_idx = global_map_.records.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (global_map_.id_map.contains(id))
            throw Exception::Parse("duplicated global id '" + id + "'", tok_.location());
        global_map_.id_map[id] = global_idx;
    }
    // exportabbr*
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "export") {
        WasmExport& exp = module_.exports.emplace_back();
        exp.name = parse_exportabbr();
        exp.index = global_idx;
        exp.desc = WasmExport::DescType::global;
    }
    // import or normal
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "import") {
        global_map_.records.emplace_back(IndexSpace::Type::Import);
        auto [imod, iname] = parse_importabbr();
        WasmImport& imp = module_.imports.emplace_back();
        imp.desc = parse_globaltype();
        imp.module = imod; imp.name = iname;
    } else {
        global_map_.records.emplace_back(IndexSpace::Type::Normal);
        WasmGlobal& global = module_.globals.emplace_back();
        global.type = parse_globaltype();
        global.init = parse_constexpr();
    }
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_exportsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("export");
    WasmExport& exp = module_.exports.emplace_back();
    exp.name = parse_string_to_bytes(tok_.expect(TokenType::String).text);
    auto [desc, idx] = parse_exportdesc();
    exp.desc = desc; exp.index = idx;
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_startsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("start");
    module_.start = parse_funcidx();
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_elemsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("elem");
    index_t elem_idx = module_.elems.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (elem_map_.contains(id))
            throw Exception::Parse("duplicated elem id '" + id + "'", tok_.location());
        elem_map_[id] = elem_idx;
    }

    WasmElem& elem = module_.elems.emplace_back();

    // Detect form:
    // 1. Active with offset: tableuse? '(' ('offset'|) constexpr ')' ...
    // 2. declare elemlist
    // 3. passive elemlist
    // 4. Active with just funcidx* (legacy): '(' 'offset'? constexpr ')' funcidx*

    // Check for '(' 'table' ... ')' tableuse
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "table") {
        // tableuse
        tok_.consume(); tok_.consume(); // '(' 'table'
        elem.mode.tableidx = parse_tableidx();
        tok_.expect(TokenType::RParen);
    }

    // Check for '(' ('offset'|constexpr-keyword) ... ')' — active
    if (tok_.peek().type == TokenType::LParen &&
        tok_.peek2().type == TokenType::Keyword &&
        (tok_.peek2().text == "offset" ||
         tok_.peek2().text == "i32.const" || tok_.peek2().text == "i64.const" ||
         tok_.peek2().text == "f32.const" || tok_.peek2().text == "f64.const" ||
         tok_.peek2().text == "ref.null" || tok_.peek2().text == "ref.func" ||
         tok_.peek2().text == "global.get")) {
        elem.mode.type = WasmElem::ElemMode::Mode::active;
        tok_.consume(); // '('
        if (tok_.peek_keyword("offset")) tok_.consume(); // 'offset'
        elem.mode.offset = parse_constexpr();
        tok_.expect(TokenType::RParen);
        if (!elem.mode.tableidx.has_value()) elem.mode.tableidx = 0;
        // funcidx* (legacy form)
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
            elem.type = RefType::funcref;
            while (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
                elem.elemlist.push_back(ConstInstr{Instr::Ref_func(parse_funcidx())});
            }
            tok_.expect(TokenType::RParen);
            return;
        }
    } else if (tok_.peek_keyword("declare")) {
        tok_.consume();
        elem.mode.type = WasmElem::ElemMode::Mode::declarative;
    } else {
        elem.mode.type = WasmElem::ElemMode::Mode::passive;
    }

    auto [reftype, elemlist] = parse_elemlist();
    elem.type = reftype;
    elem.elemlist = std::move(elemlist);
    tok_.expect(TokenType::RParen);
}

void ParseContext::parse_datasection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("data");
    index_t data_idx = module_.datas.size();
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        if (data_map_.contains(id))
            throw Exception::Parse("duplicated data id '" + id + "'", tok_.location());
        data_map_[id] = data_idx;
    }

    WasmData& data = module_.datas.emplace_back();
    data.mode.type = WasmData::DataMode::Mode::passive;

    // Check for memuse or offset
    if (tok_.peek().type == TokenType::LParen) {
        const Token& kw = tok_.peek2();
        if (kw.type == TokenType::Keyword) {
            if (kw.text == "memory") {
                // memuse: '(' 'memory' memidx ')'
                tok_.consume(); tok_.consume();
                data.mode.memidx = parse_memidx();
                tok_.expect(TokenType::RParen);
            }
            // offset clause
            if (tok_.peek().type == TokenType::LParen &&
                tok_.peek2().type == TokenType::Keyword &&
                (tok_.peek2().text == "offset" ||
                 tok_.peek2().text == "i32.const" || tok_.peek2().text == "i64.const" ||
                 tok_.peek2().text == "f32.const" || tok_.peek2().text == "f64.const" ||
                 tok_.peek2().text == "ref.null" || tok_.peek2().text == "ref.func" ||
                 tok_.peek2().text == "global.get")) {
                tok_.consume(); // '('
                if (tok_.peek_keyword("offset")) tok_.consume();
                data.mode.offset = parse_constexpr();
                tok_.expect(TokenType::RParen);
                data.mode.type = WasmData::DataMode::Mode::active;
                if (!data.mode.memidx.has_value()) data.mode.memidx = 0;
            }
        }
    }

    // String* — data bytes
    while (tok_.peek().type == TokenType::String) {
        std::string bytes = parse_string_to_bytes(tok_.consume().text);
        size_t orig = data.init.size();
        data.init.resize(orig + bytes.size());
        std::memcpy(data.init.data() + orig, bytes.data(), bytes.size());
    }
    tok_.expect(TokenType::RParen);
}

// ── Tag section ───────────────────────────────────────────────────────────────

void ParseContext::parse_tagsection() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("tag");
    // optional Id
    if (tok_.peek().type == TokenType::Id) {
        std::string id = tok_.consume().text;
        tag_map_[id] = module_.tags.size();
    }
    // typeuse: (type N) or inline params
    index_t typeidx = 0;
    if (is_typeuse_start())
        typeidx = parse_typeuse();
    module_.tags.push_back(typeidx);
    tok_.expect(TokenType::RParen);
}
