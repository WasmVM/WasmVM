// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "ParseContext.hpp"
#include <WasmVM.hpp>

#include <algorithm>

using namespace WasmVM;
using namespace WasmVM::Parse;

// ── Instruction dispatch ──────────────────────────────────────────────────────

std::vector<WasmInstr> ParseContext::parse_instr() {
    if (tok_.peek().type == TokenType::LParen)
        return parse_foldedinstr();
    if (tok_.peek().type == TokenType::Keyword) {
        std::string kw = tok_.peek().text;
        if (kw == "block" || kw == "loop" || kw == "if" || kw == "try_table")
            return parse_blockinstr(kw);
    }
    return {parse_plaininstr()};
}

WasmInstr ParseContext::parse_plaininstr() {
    Token kw = tok_.expect(TokenType::Keyword, "instruction");
    const std::string& name = kw.text;
    if (name == "unreachable" || name == "nop" || name == "return" ||
        name == "br" || name == "br_if" || name == "br_table" ||
        name == "call" || name == "call_indirect" ||
        name == "call_ref" || name == "return_call" ||
        name == "return_call_indirect" || name == "return_call_ref" ||
        name == "throw" || name == "throw_ref")
        return parse_controlinstr(name);
    if (name == "ref.null" || name == "ref.is_null" || name == "ref.func" ||
        name == "ref.eq" || name == "ref.as_non_null" || name == "ref.i31" ||
        name == "ref.test" || name == "ref.cast" ||
        name == "i31.get_s" || name == "i31.get_u" ||
        name == "any.convert_extern" || name == "extern.convert_any" ||
        name == "br_on_null" || name == "br_on_non_null" ||
        name == "br_on_cast" || name == "br_on_cast_fail")
        return parse_referenceinstr(name);
    if (name == "drop" || name == "select")
        return parse_parametricinstr(name);
    if (name.starts_with("local.") || name.starts_with("global."))
        return parse_variableinstr(name);
    if (name.starts_with("table.") || name == "elem.drop")
        return parse_tableinstr(name);
    if (name == "data.drop" || name.starts_with("memory.") ||
        name.find(".load") != std::string::npos || name.find(".store") != std::string::npos)
        return parse_memoryinstr(name);
    if (name.starts_with("struct.") || name.starts_with("array."))
        return parse_gcinstr(name);
    return parse_numericinstr(name);
}

// ── Block instructions (non-folded form) ─────────────────────────────────────

std::vector<WasmInstr> ParseContext::parse_blockinstr(const std::string& kw) {
    tok_.consume(); // consume 'block'/'loop'/'if'/'try_table'

    // Increment block level FIRST, then register the label at that level
    block_level_ += 1;
    std::string label = parse_label();

    std::optional<index_t> type;
    if (is_typeuse_start()) type = parse_typeuse();

    // try_table: parse catch entries before body
    if (kw == "try_table") {
        Instr::Try_table tt;
        tt.type = type;
        while (tok_.peek().type == TokenType::LParen) {
            const Token& peek2 = tok_.peek2();
            if (peek2.type != TokenType::Keyword) break;
            const std::string& ck = peek2.text;
            if (ck != "catch" && ck != "catch_ref" && ck != "catch_all" && ck != "catch_all_ref")
                break;
            tok_.consume(); // '('
            tok_.consume(); // catch keyword
            Instr::TryCatchEntry entry;
            if      (ck == "catch")     entry.kind = Instr::TryCatchEntry::Catch;
            else if (ck == "catch_ref") entry.kind = Instr::TryCatchEntry::CatchRef;
            else if (ck == "catch_all") entry.kind = Instr::TryCatchEntry::CatchAll;
            else                        entry.kind = Instr::TryCatchEntry::CatchAllRef;
            if (entry.kind == Instr::TryCatchEntry::Catch || entry.kind == Instr::TryCatchEntry::CatchRef)
                entry.tag_idx = parse_tagidx();
            else
                entry.tag_idx = 0;
            entry.label_idx = parse_labelidx();
            tok_.expect(TokenType::RParen);
            tt.catches.push_back(entry);
        }
        std::vector<WasmInstr> instrs;
        instrs.emplace_back(tt);
        while (!tok_.at_eof() && !tok_.peek_keyword("end")) {
            auto new_instrs = parse_instr();
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
        block_level_ -= 1;
        tok_.expect_keyword("end");
        if (tok_.peek().type == TokenType::Id) {
            Token closing = tok_.consume();
            if (!label.empty() && closing.text != label)
                throw Exception::Parse("id '" + closing.text + "' does not match block label",
                                       {closing.line, closing.column});
        }
        instrs.emplace_back(Instr::End());
        return instrs;
    }

    std::vector<WasmInstr> instrs;
    if      (kw == "if")    instrs.emplace_back(Instr::If(type));
    else if (kw == "block") instrs.emplace_back(Instr::Block(type));
    else                    instrs.emplace_back(Instr::Loop(type));

    // then-body: instr* until 'else' or 'end'
    while (!tok_.at_eof() && !tok_.peek_keyword("end") && !tok_.peek_keyword("else")) {
        auto new_instrs = parse_instr();
        instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
    }
    block_level_ -= 1;

    // Else clause (always emitted for 'if' per original behavior)
    if (kw == "if") {
        instrs.emplace_back(Instr::Else());
        block_level_ += 1;
        if (tok_.peek_keyword("else")) {
            tok_.consume(); // 'else'
            if (tok_.peek().type == TokenType::Id) tok_.consume(); // optional label
            while (!tok_.at_eof() && !tok_.peek_keyword("end")) {
                auto new_instrs = parse_instr();
                instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
            }
        }
        block_level_ -= 1;
    }

    tok_.expect_keyword("end");
    if (tok_.peek().type == TokenType::Id) {
        Token closing = tok_.consume();
        if (!label.empty() && closing.text != label)
            throw Exception::Parse("id '" + closing.text + "' does not match block label",
                                   {closing.line, closing.column});
    }
    instrs.emplace_back(Instr::End());
    return instrs;
}

// ── Folded instructions ───────────────────────────────────────────────────────

std::vector<WasmInstr> ParseContext::parse_foldedinstr() {
    tok_.expect(TokenType::LParen);

    const Token& kw = tok_.peek();
    if (kw.type != TokenType::Keyword)
        throw Exception::Parse("expected instruction keyword in folded form", tok_.location());
    const std::string name = kw.text; // copy before consuming

    std::vector<WasmInstr> instrs;

    if (name == "block" || name == "loop") {
        tok_.consume(); // consume 'block'/'loop'
        block_level_ += 1;
        std::string label = parse_label();
        std::optional<index_t> type;
        if (is_typeuse_start()) type = parse_typeuse();
        if (name == "block") instrs.emplace_back(Instr::Block(type));
        else                 instrs.emplace_back(Instr::Loop(type));
        while (tok_.peek().type != TokenType::RParen && !tok_.at_eof()) {
            auto new_instrs = parse_instr();
            instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
        }
        block_level_ -= 1;
        instrs.emplace_back(Instr::End());
    } else if (name == "if") {
        tok_.consume(); // consume 'if'
        block_level_ += 1;
        std::string label = parse_label();
        std::optional<index_t> type;
        if (is_typeuse_start()) type = parse_typeuse();

        // foldedinstr* (condition values evaluated before the If)
        std::vector<WasmInstr> condition;
        while (tok_.peek().type == TokenType::LParen) {
            const Token& inner = tok_.peek2();
            if (inner.type == TokenType::Keyword &&
                (inner.text == "then" || inner.text == "else")) break;
            auto fi = parse_foldedinstr();
            condition.insert(condition.end(), fi.begin(), fi.end());
        }
        instrs.insert(instrs.end(), condition.begin(), condition.end());
        instrs.emplace_back(Instr::If(type));

        // '(' 'then' instr* ')'
        if (tok_.peek().type == TokenType::LParen &&
            tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "then") {
            tok_.consume(); tok_.consume(); // '(' 'then'
            while (tok_.peek().type != TokenType::RParen && !tok_.at_eof()) {
                auto new_instrs = parse_instr();
                instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
            }
            tok_.expect(TokenType::RParen); // close then
        }
        block_level_ -= 1;

        instrs.emplace_back(Instr::Else());
        block_level_ += 1;

        // optional '(' 'else' elseinstr* ')'
        if (tok_.peek().type == TokenType::LParen &&
            tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "else") {
            tok_.consume(); tok_.consume(); // '(' 'else'
            while (tok_.peek().type != TokenType::RParen && !tok_.at_eof()) {
                auto new_instrs = parse_instr();
                instrs.insert(instrs.end(), new_instrs.begin(), new_instrs.end());
            }
            tok_.expect(TokenType::RParen); // close else
        }
        block_level_ -= 1;
        instrs.emplace_back(Instr::End());
    } else {
        // Plain folded: '(' plaininstr foldedinstr* ')'
        // parse_plaininstr() will consume the keyword + inline operands
        WasmInstr head = parse_plaininstr();
        // collect nested folded arguments (executed before head)
        while (tok_.peek().type == TokenType::LParen) {
            auto fi = parse_foldedinstr();
            instrs.insert(instrs.end(), fi.begin(), fi.end());
        }
        instrs.push_back(std::move(head));
    }

    tok_.expect(TokenType::RParen);
    return instrs;
}

// ── Control instructions ──────────────────────────────────────────────────────

WasmInstr ParseContext::parse_controlinstr(const std::string& name) {
    if (name == "unreachable") return Instr::Unreachable();
    if (name == "nop")         return Instr::Nop();
    if (name == "return")      return Instr::Return();
    if (name == "br")          return Instr::Br(parse_labelidx());
    if (name == "br_if")       return Instr::Br_if(parse_labelidx());
    if (name == "br_table") {
        Instr::Br_table instr;
        while (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
            instr.indices.push_back(parse_labelidx());
        return instr;
    }
    if (name == "call")          return Instr::Call(parse_funcidx());
    if (name == "call_ref")      return Instr::Call_ref(parse_typeidx());
    if (name == "return_call")   return Instr::Return_call(parse_funcidx());
    if (name == "return_call_ref") return Instr::Return_call_ref(parse_typeidx());
    if (name == "throw")         return Instr::Throw(parse_tagidx());
    if (name == "throw_ref")     return Instr::Throw_ref();
    if (name == "return_call_indirect") {
        index_t tableidx = 0;
        if ((tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
            && !is_typeuse_start())
            tableidx = parse_tableidx();
        return Instr::Return_call_indirect(tableidx, parse_typeuse());
    }
    // call_indirect: tableidx? typeuse
    index_t tableidx = 0;
    if ((tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
        && !is_typeuse_start()) {
        tableidx = parse_tableidx();
    }
    index_t typeidx = parse_typeuse();
    return Instr::Call_indirect(tableidx, typeidx);
}

// ── Reference instructions ────────────────────────────────────────────────────

std::pair<bool, int32_t> ParseContext::parse_reftype_annot() {
    tok_.expect(TokenType::LParen);
    tok_.expect_keyword("ref");
    bool nullable = false;
    if (tok_.peek_keyword("null")) { tok_.consume(); nullable = true; }
    int32_t heaptype;
    if (tok_.peek().type == TokenType::Integer) {
        heaptype = (int32_t)parse_u32();
    } else {
        Token ht = tok_.expect(TokenType::Keyword, "heap type");
        if      (ht.text == "func")   heaptype = -16;
        else if (ht.text == "extern") heaptype = -17;
        else if (ht.text == "any")    heaptype = -18;
        else if (ht.text == "eq")     heaptype = -19;
        else if (ht.text == "i31")    heaptype = -20;
        else if (ht.text == "struct") heaptype = -21;
        else if (ht.text == "array")  heaptype = -22;
        else throw Exception::Parse("unknown heap type '" + ht.text + "'", {ht.line, ht.column});
    }
    tok_.expect(TokenType::RParen);
    return {nullable, heaptype};
}

WasmInstr ParseContext::parse_referenceinstr(const std::string& name) {
    if (name == "ref.is_null")       return Instr::Ref_is_null();
    if (name == "ref.func")          return Instr::Ref_func(parse_funcidx());
    if (name == "ref.eq")            return Instr::Ref_eq();
    if (name == "ref.as_non_null")   return Instr::Ref_as_non_null();
    if (name == "ref.i31")           return Instr::Ref_i31();
    if (name == "i31.get_s")         return Instr::I31_get_s();
    if (name == "i31.get_u")         return Instr::I31_get_u();
    if (name == "any.convert_extern") return Instr::Any_convert_extern();
    if (name == "extern.convert_any") return Instr::Extern_convert_any();
    if (name == "br_on_null")        return Instr::Br_on_null(parse_labelidx());
    if (name == "br_on_non_null")    return Instr::Br_on_non_null(parse_labelidx());
    if (name == "ref.test") {
        auto [nullable, ht] = parse_reftype_annot();
        return nullable ? WasmInstr(Instr::Ref_test_null(ht)) : WasmInstr(Instr::Ref_test(ht));
    }
    if (name == "ref.cast") {
        auto [nullable, ht] = parse_reftype_annot();
        return nullable ? WasmInstr(Instr::Ref_cast_null(ht)) : WasmInstr(Instr::Ref_cast(ht));
    }
    if (name == "br_on_cast" || name == "br_on_cast_fail") {
        index_t labelidx = parse_labelidx();
        auto [src_nullable, src_ht] = parse_reftype_annot();
        auto [dst_nullable, dst_ht] = parse_reftype_annot();
        if (name == "br_on_cast")
            return Instr::Br_on_cast(labelidx, src_nullable, dst_nullable, src_ht, dst_ht);
        return Instr::Br_on_cast_fail(labelidx, src_nullable, dst_nullable, src_ht, dst_ht);
    }
    // ref.null — heaptype keyword
    Token ref = tok_.expect(TokenType::Keyword, "heap type");
    if (ref.text == "func")   return Instr::Ref_null(RefType::funcref);
    if (ref.text == "extern") return Instr::Ref_null(RefType::externref);
    throw Exception::Parse("expected heap type after ref.null", {ref.line, ref.column});
}

// ── Parametric instructions ───────────────────────────────────────────────────

WasmInstr ParseContext::parse_parametricinstr(const std::string& name) {
    if (name == "drop") return Instr::Drop();
    // select result*
    Instr::Select instr;
    while (tok_.peek().type == TokenType::LParen &&
           tok_.peek2().type == TokenType::Keyword && tok_.peek2().text == "result") {
        auto results = parse_result();
        instr.valtypes.insert(instr.valtypes.end(), results.begin(), results.end());
    }
    return instr;
}

// ── Variable instructions ─────────────────────────────────────────────────────

WasmInstr ParseContext::parse_variableinstr(const std::string& name) {
    if (name == "local.get") return Instr::Local_get(parse_localidx());
    if (name == "local.set") return Instr::Local_set(parse_localidx());
    if (name == "local.tee") return Instr::Local_tee(parse_localidx());
    if (name == "global.get") return Instr::Global_get(parse_globalidx());
    return Instr::Global_set(parse_globalidx());
}

// ── Table instructions ────────────────────────────────────────────────────────

WasmInstr ParseContext::parse_tableinstr(const std::string& name) {
    if (name == "elem.drop") return Instr::Elem_drop(parse_elemidx());
    if (name == "table.copy") {
        index_t dst = 0, src = 0;
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
            dst = parse_tableidx();
            src = parse_tableidx();
        }
        return Instr::Table_copy(dst, src);
    }
    index_t tableidx = 0;
    if (name != "table.init") {
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
            tableidx = parse_tableidx();
    } else {
        // table.init tableidx? elemidx — need to distinguish
        // If two indices follow, first is tableidx; if one, it's elemidx
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
            tableidx = parse_u32(); // tentative: first index
            // peek to see if another index follows (then the first was tableidx)
            if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
                // two indices: tableidx elemidx
                return Instr::Table_init((index_t)tableidx, parse_elemidx());
            }
            // only one index: it was the elemidx
            return Instr::Table_init(0, (index_t)tableidx);
        }
        return Instr::Table_init(0, 0);
    }
    if (name == "table.get")  return Instr::Table_get(tableidx);
    if (name == "table.set")  return Instr::Table_set(tableidx);
    if (name == "table.size") return Instr::Table_size(tableidx);
    if (name == "table.grow") return Instr::Table_grow(tableidx);
    if (name == "table.fill") return Instr::Table_fill(tableidx);
    return Instr::Table_fill(tableidx); // fallback
}

// ── Memarg ────────────────────────────────────────────────────────────────────

std::pair<offset_t, std::optional<align_t>> ParseContext::parse_memarg() {
    std::pair<offset_t, std::optional<align_t>> result{0, std::nullopt};
    if (tok_.peek().type == TokenType::Offset)
        result.first = (offset_t)std::stoull(tok_.consume().text);
    if (tok_.peek().type == TokenType::Align)
        result.second = std::stoi(tok_.consume().text);
    return result;
}

// ── Memory instructions ───────────────────────────────────────────────────────

WasmInstr ParseContext::parse_memoryinstr(const std::string& name) {
    if (name == "data.drop") return Instr::Data_drop(parse_dataidx());

    if (name == "memory.copy") {
        index_t dst = 0, src = 0;
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
            dst = parse_memidx(); src = parse_memidx();
        }
        return Instr::Memory_copy(dst, src);
    }

    // memory.{size,grow,fill,init}: optional memidx
    if (name.starts_with("memory.")) {
        index_t memidx = 0;
        if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id)
            memidx = parse_memidx();
        if (name == "memory.size") return Instr::Memory_size(memidx);
        if (name == "memory.grow") return Instr::Memory_grow(memidx);
        if (name == "memory.fill") return Instr::Memory_fill(memidx);
        if (name == "memory.init") {
            // memidx? dataidx
            if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
                // had memidx, now get dataidx
                return Instr::Memory_init(memidx, parse_dataidx());
            }
            // only one index: it was dataidx
            return Instr::Memory_init(0, memidx);
        }
    }

    // Load/store: optional memidx then memarg
    index_t memidx = 0;
    if (tok_.peek().type == TokenType::Integer || tok_.peek().type == TokenType::Id) {
        // Could be memidx before offset=, but memidx is only present if followed by another token
        // that is also an index or the next token is offset=/align=
        // Simple heuristic: if the next token is an integer/id and no offset/align follows immediately
        // For now: if tok[0] is integer/id and tok[1] is integer/id/offset/align/RParen/EOF → tok[0] is memidx
        // Actually the WAT spec says memidx is optional and only present in multi-memory proposal.
        // For simplicity: just try to parse memidx if there's an integer/id before offset= or end
        // The tokenizer returns Offset/Align for those, so any integer/id here would be memidx.
        // But we need to be careful — if there's only one integer and it could be the value of
        // i32.const etc. that came before... no, those are not in memoryinstr context.
        // The only integer/id that can appear before offset= in a memory instr is memidx.
        memidx = parse_u32();
        // Check if there's actually more (otherwise treat as... hmm)
        // Actually per grammar: memidx? is only for multi-memory. In practice single-memory
        // modules don't use it. But we should support it.
        // The ambiguity is: if there's ONE integer before offset/align/end, is it memidx or not?
        // In single-memory WAT: no memidx appears. In multi-memory: memidx appears.
        // The grammar says "memidx?" so it's optional. Since an integer here is always memidx
        // (there's nothing else an integer would be), we're fine.
    }
    auto [offset, align] = parse_memarg();

    // Default alignment based on instruction width
    if (!align.has_value()) {
        if (name.ends_with("8") || name.ends_with("8_s") || name.ends_with("8_u"))   align = 0;
        else if (name.ends_with("16") || name.ends_with("16_s") || name.ends_with("16_u")) align = 1;
        else if (name.ends_with("32") || name.ends_with("32_s") || name.ends_with("32_u")) align = 2;
    }

    if (name.starts_with("i32")) {
        if (!align.has_value()) align = 2;
        if (name.ends_with("load"))     return Instr::I32_load(memidx, offset, align.value());
        if (name.ends_with("load8_s"))  return Instr::I32_load8_s(memidx, offset, align.value());
        if (name.ends_with("load8_u"))  return Instr::I32_load8_u(memidx, offset, align.value());
        if (name.ends_with("load16_s")) return Instr::I32_load16_s(memidx, offset, align.value());
        if (name.ends_with("load16_u")) return Instr::I32_load16_u(memidx, offset, align.value());
        if (name.ends_with("store"))    return Instr::I32_store(memidx, offset, align.value());
        if (name.ends_with("store8"))   return Instr::I32_store8(memidx, offset, align.value());
        return Instr::I32_store16(memidx, offset, align.value());
    } else if (name.starts_with("i64")) {
        if (!align.has_value()) align = 3;
        if (name.ends_with("load"))     return Instr::I64_load(memidx, offset, align.value());
        if (name.ends_with("load8_s"))  return Instr::I64_load8_s(memidx, offset, align.value());
        if (name.ends_with("load8_u"))  return Instr::I64_load8_u(memidx, offset, align.value());
        if (name.ends_with("load16_s")) return Instr::I64_load16_s(memidx, offset, align.value());
        if (name.ends_with("load16_u")) return Instr::I64_load16_u(memidx, offset, align.value());
        if (name.ends_with("load32_s")) return Instr::I64_load32_s(memidx, offset, align.value());
        if (name.ends_with("load32_u")) return Instr::I64_load32_u(memidx, offset, align.value());
        if (name.ends_with("store"))    return Instr::I64_store(memidx, offset, align.value());
        if (name.ends_with("store8"))   return Instr::I64_store8(memidx, offset, align.value());
        if (name.ends_with("store16"))  return Instr::I64_store16(memidx, offset, align.value());
        return Instr::I64_store32(memidx, offset, align.value());
    } else if (name.starts_with("f32")) {
        if (!align.has_value()) align = 2;
        if (name.ends_with("load")) return Instr::F32_load(memidx, offset, align.value());
        return Instr::F32_store(memidx, offset, align.value());
    } else {
        if (!align.has_value()) align = 3;
        if (name.ends_with("load")) return Instr::F64_load(memidx, offset, align.value());
        return Instr::F64_store(memidx, offset, align.value());
    }
}

// ── Numeric instructions ──────────────────────────────────────────────────────

WasmInstr ParseContext::parse_numericinstr(const std::string& name) {
    if (name == "i32.const") return Instr::I32_const(parse_i32());
    if (name == "i64.const") return Instr::I64_const(parse_i64());
    if (name == "f32.const") return Instr::F32_const(parse_f32());
    if (name == "f64.const") return Instr::F64_const(parse_f64());

    if (name.starts_with("i32.")) {
        if (name == "i32.clz")              return Instr::I32_clz();
        if (name == "i32.ctz")              return Instr::I32_ctz();
        if (name == "i32.popcnt")           return Instr::I32_popcnt();
        if (name == "i32.add")              return Instr::I32_add();
        if (name == "i32.sub")              return Instr::I32_sub();
        if (name == "i32.mul")              return Instr::I32_mul();
        if (name == "i32.div_s")            return Instr::I32_div_s();
        if (name == "i32.div_u")            return Instr::I32_div_u();
        if (name == "i32.rem_s")            return Instr::I32_rem_s();
        if (name == "i32.rem_u")            return Instr::I32_rem_u();
        if (name == "i32.and")              return Instr::I32_and();
        if (name == "i32.or")               return Instr::I32_or();
        if (name == "i32.xor")              return Instr::I32_xor();
        if (name == "i32.shl")              return Instr::I32_shl();
        if (name == "i32.shr_s")            return Instr::I32_shr_s();
        if (name == "i32.shr_u")            return Instr::I32_shr_u();
        if (name == "i32.rotl")             return Instr::I32_rotl();
        if (name == "i32.rotr")             return Instr::I32_rotr();
        if (name == "i32.eqz")              return Instr::I32_eqz();
        if (name == "i32.eq")               return Instr::I32_eq();
        if (name == "i32.ne")               return Instr::I32_ne();
        if (name == "i32.lt_s")             return Instr::I32_lt_s();
        if (name == "i32.lt_u")             return Instr::I32_lt_u();
        if (name == "i32.gt_s")             return Instr::I32_gt_s();
        if (name == "i32.gt_u")             return Instr::I32_gt_u();
        if (name == "i32.le_s")             return Instr::I32_le_s();
        if (name == "i32.le_u")             return Instr::I32_le_u();
        if (name == "i32.ge_s")             return Instr::I32_ge_s();
        if (name == "i32.ge_u")             return Instr::I32_ge_u();
        if (name == "i32.wrap_i64")         return Instr::I32_wrap_i64();
        if (name == "i32.trunc_f32_s")      return Instr::I32_trunc_f32_s();
        if (name == "i32.trunc_f32_u")      return Instr::I32_trunc_f32_u();
        if (name == "i32.trunc_f64_s")      return Instr::I32_trunc_f64_s();
        if (name == "i32.trunc_f64_u")      return Instr::I32_trunc_f64_u();
        if (name == "i32.trunc_sat_f32_s")  return Instr::I32_trunc_sat_f32_s();
        if (name == "i32.trunc_sat_f32_u")  return Instr::I32_trunc_sat_f32_u();
        if (name == "i32.trunc_sat_f64_s")  return Instr::I32_trunc_sat_f64_s();
        if (name == "i32.trunc_sat_f64_u")  return Instr::I32_trunc_sat_f64_u();
        if (name == "i32.reinterpret_f32")  return Instr::I32_reinterpret_f32();
        if (name == "i32.extend8_s")        return Instr::I32_extend8_s();
        if (name == "i32.extend16_s")       return Instr::I32_extend16_s();
    } else if (name.starts_with("i64.")) {
        if (name == "i64.clz")              return Instr::I64_clz();
        if (name == "i64.ctz")              return Instr::I64_ctz();
        if (name == "i64.popcnt")           return Instr::I64_popcnt();
        if (name == "i64.add")              return Instr::I64_add();
        if (name == "i64.sub")              return Instr::I64_sub();
        if (name == "i64.mul")              return Instr::I64_mul();
        if (name == "i64.div_s")            return Instr::I64_div_s();
        if (name == "i64.div_u")            return Instr::I64_div_u();
        if (name == "i64.rem_s")            return Instr::I64_rem_s();
        if (name == "i64.rem_u")            return Instr::I64_rem_u();
        if (name == "i64.and")              return Instr::I64_and();
        if (name == "i64.or")               return Instr::I64_or();
        if (name == "i64.xor")              return Instr::I64_xor();
        if (name == "i64.shl")              return Instr::I64_shl();
        if (name == "i64.shr_s")            return Instr::I64_shr_s();
        if (name == "i64.shr_u")            return Instr::I64_shr_u();
        if (name == "i64.rotl")             return Instr::I64_rotl();
        if (name == "i64.rotr")             return Instr::I64_rotr();
        if (name == "i64.eqz")              return Instr::I64_eqz();
        if (name == "i64.eq")               return Instr::I64_eq();
        if (name == "i64.ne")               return Instr::I64_ne();
        if (name == "i64.lt_s")             return Instr::I64_lt_s();
        if (name == "i64.lt_u")             return Instr::I64_lt_u();
        if (name == "i64.gt_s")             return Instr::I64_gt_s();
        if (name == "i64.gt_u")             return Instr::I64_gt_u();
        if (name == "i64.le_s")             return Instr::I64_le_s();
        if (name == "i64.le_u")             return Instr::I64_le_u();
        if (name == "i64.ge_s")             return Instr::I64_ge_s();
        if (name == "i64.ge_u")             return Instr::I64_ge_u();
        if (name == "i64.extend_i32_s")     return Instr::I64_extend_i32_s();
        if (name == "i64.extend_i32_u")     return Instr::I64_extend_i32_u();
        if (name == "i64.trunc_f32_s")      return Instr::I64_trunc_f32_s();
        if (name == "i64.trunc_f32_u")      return Instr::I64_trunc_f32_u();
        if (name == "i64.trunc_f64_s")      return Instr::I64_trunc_f64_s();
        if (name == "i64.trunc_f64_u")      return Instr::I64_trunc_f64_u();
        if (name == "i64.trunc_sat_f32_s")  return Instr::I64_trunc_sat_f32_s();
        if (name == "i64.trunc_sat_f32_u")  return Instr::I64_trunc_sat_f32_u();
        if (name == "i64.trunc_sat_f64_s")  return Instr::I64_trunc_sat_f64_s();
        if (name == "i64.trunc_sat_f64_u")  return Instr::I64_trunc_sat_f64_u();
        if (name == "i64.reinterpret_f64")  return Instr::I64_reinterpret_f64();
        if (name == "i64.extend8_s")        return Instr::I64_extend8_s();
        if (name == "i64.extend16_s")       return Instr::I64_extend16_s();
        if (name == "i64.extend32_s")       return Instr::I64_extend32_s();
    } else if (name.starts_with("f32.")) {
        if (name == "f32.abs")              return Instr::F32_abs();
        if (name == "f32.neg")              return Instr::F32_neg();
        if (name == "f32.ceil")             return Instr::F32_ceil();
        if (name == "f32.floor")            return Instr::F32_floor();
        if (name == "f32.trunc")            return Instr::F32_trunc();
        if (name == "f32.nearest")          return Instr::F32_nearest();
        if (name == "f32.sqrt")             return Instr::F32_sqrt();
        if (name == "f32.add")              return Instr::F32_add();
        if (name == "f32.sub")              return Instr::F32_sub();
        if (name == "f32.mul")              return Instr::F32_mul();
        if (name == "f32.div")              return Instr::F32_div();
        if (name == "f32.min")              return Instr::F32_min();
        if (name == "f32.max")              return Instr::F32_max();
        if (name == "f32.copysign")         return Instr::F32_copysign();
        if (name == "f32.eq")               return Instr::F32_eq();
        if (name == "f32.ne")               return Instr::F32_ne();
        if (name == "f32.lt")               return Instr::F32_lt();
        if (name == "f32.gt")               return Instr::F32_gt();
        if (name == "f32.le")               return Instr::F32_le();
        if (name == "f32.ge")               return Instr::F32_ge();
        if (name == "f32.convert_i32_s")    return Instr::F32_convert_i32_s();
        if (name == "f32.convert_i32_u")    return Instr::F32_convert_i32_u();
        if (name == "f32.convert_i64_s")    return Instr::F32_convert_i64_s();
        if (name == "f32.convert_i64_u")    return Instr::F32_convert_i64_u();
        if (name == "f32.demote_f64")       return Instr::F32_demote_f64();
        if (name == "f32.reinterpret_i32")  return Instr::F32_reinterpret_i32();
    } else if (name.starts_with("f64.")) {
        if (name == "f64.abs")              return Instr::F64_abs();
        if (name == "f64.neg")              return Instr::F64_neg();
        if (name == "f64.ceil")             return Instr::F64_ceil();
        if (name == "f64.floor")            return Instr::F64_floor();
        if (name == "f64.trunc")            return Instr::F64_trunc();
        if (name == "f64.nearest")          return Instr::F64_nearest();
        if (name == "f64.sqrt")             return Instr::F64_sqrt();
        if (name == "f64.add")              return Instr::F64_add();
        if (name == "f64.sub")              return Instr::F64_sub();
        if (name == "f64.mul")              return Instr::F64_mul();
        if (name == "f64.div")              return Instr::F64_div();
        if (name == "f64.min")              return Instr::F64_min();
        if (name == "f64.max")              return Instr::F64_max();
        if (name == "f64.copysign")         return Instr::F64_copysign();
        if (name == "f64.eq")               return Instr::F64_eq();
        if (name == "f64.ne")               return Instr::F64_ne();
        if (name == "f64.lt")               return Instr::F64_lt();
        if (name == "f64.gt")               return Instr::F64_gt();
        if (name == "f64.le")               return Instr::F64_le();
        if (name == "f64.ge")               return Instr::F64_ge();
        if (name == "f64.convert_i32_s")    return Instr::F64_convert_i32_s();
        if (name == "f64.convert_i32_u")    return Instr::F64_convert_i32_u();
        if (name == "f64.convert_i64_s")    return Instr::F64_convert_i64_s();
        if (name == "f64.convert_i64_u")    return Instr::F64_convert_i64_u();
        if (name == "f64.promote_f32")      return Instr::F64_promote_f32();
        if (name == "f64.reinterpret_i64")  return Instr::F64_reinterpret_i64();
    }
    throw Exception::Parse("unknown instruction '" + name + "'", tok_.location());
}

// ── GC struct/array instructions ──────────────────────────────────────────────

WasmInstr ParseContext::parse_gcinstr(const std::string& name) {
    // Struct
    if (name == "struct.new")         return Instr::Struct_new(parse_typeidx());
    if (name == "struct.new_default") return Instr::Struct_new_default(parse_typeidx());
    if (name == "struct.get")   { index_t t = parse_typeidx(); return Instr::Struct_get(t, parse_u32()); }
    if (name == "struct.get_s") { index_t t = parse_typeidx(); return Instr::Struct_get_s(t, parse_u32()); }
    if (name == "struct.get_u") { index_t t = parse_typeidx(); return Instr::Struct_get_u(t, parse_u32()); }
    if (name == "struct.set")   { index_t t = parse_typeidx(); return Instr::Struct_set(t, parse_u32()); }
    // Array
    if (name == "array.new")         return Instr::Array_new(parse_typeidx());
    if (name == "array.new_default") return Instr::Array_new_default(parse_typeidx());
    if (name == "array.new_fixed")   { index_t t = parse_typeidx(); return Instr::Array_new_fixed(t, parse_u32()); }
    if (name == "array.new_data")    { index_t t = parse_typeidx(); return Instr::Array_new_data(t, parse_dataidx()); }
    if (name == "array.new_elem")    { index_t t = parse_typeidx(); return Instr::Array_new_elem(t, parse_elemidx()); }
    if (name == "array.get")         return Instr::Array_get(parse_typeidx());
    if (name == "array.get_s")       return Instr::Array_get_s(parse_typeidx());
    if (name == "array.get_u")       return Instr::Array_get_u(parse_typeidx());
    if (name == "array.set")         return Instr::Array_set(parse_typeidx());
    if (name == "array.len")         return Instr::Array_len();
    if (name == "array.fill")        return Instr::Array_fill(parse_typeidx());
    if (name == "array.copy")        { index_t d = parse_typeidx(); return Instr::Array_copy(d, parse_typeidx()); }
    if (name == "array.init_data")   { index_t t = parse_typeidx(); return Instr::Array_init_data(t, parse_dataidx()); }
    if (name == "array.init_elem")   { index_t t = parse_typeidx(); return Instr::Array_init_elem(t, parse_elemidx()); }
    throw Exception::Parse("unknown GC instruction '" + name + "'", tok_.location());
}
