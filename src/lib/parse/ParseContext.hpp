// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_Parse_ParseContext
#define WASMVM_Parse_ParseContext

#include <WasmVM.hpp>
#include "Tokenizer.hpp"

#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>
#include <tuple>
#include <utility>

namespace WasmVM {
namespace Parse {

class ParseContext {
public:
    explicit ParseContext(Tokenizer& tok);

    // Main entry point: parses the module and runs post_process, returns WasmModule.
    WasmModule parse();

private:
    // ── State (mirrors Visitor) ──────────────────────────────────────────────
    struct IndexSpace {
        enum class Type { Import, Normal };
        std::vector<Type> records;
        std::map<std::string, index_t> id_map;
    };

    WasmModule module_;
    std::map<std::string, index_t> type_map_;
    std::map<std::string, index_t> elem_map_;
    std::map<std::string, index_t> data_map_;
    std::map<std::string, index_t> local_map_;
    std::map<std::string, index_t> label_map_;
    IndexSpace func_map_, table_map_, mem_map_, global_map_, tag_map_;
    // types_ stores (SubType, member-id-map) pairs; module_.types is populated
    // in parse(). The map holds the param ids of a function type or the field
    // ids of a struct type, whichever the entry is.
    std::vector<std::pair<SubType, std::map<std::string, index_t>>> types_;
    // A supertype named by an id that was not yet defined: inside a (rec ...)
    // group a member may name a later member. Resolved when the group closes.
    // (type index, position in supers, the id token)
    std::vector<std::tuple<index_t, size_t, Token>> pending_supers_;
    index_t block_level_ = 0;

    Tokenizer& tok_;

    // ── Module / section parsers ────────────────────────────────────────────
    void parse_module();
    void parse_modulefield();

    void parse_typesection();
    void parse_recsection();
    void parse_importsection();
    void parse_funcsection();
    void parse_tablesection();
    void parse_memorysection();
    void parse_globalsection();
    void parse_exportsection();
    void parse_startsection();
    void parse_elemsection();
    void parse_datasection();
    void parse_tagsection();

    // ── Type / structural sub-parsers ────────────────────────────────────────
    ValueType  parse_valtype();
    // A value type that has to be a reference: a table's or an elem
    // segment's element type.
    ValueType  parse_reference_valtype(const char* what);
    // True when the next tokens start a value type: a keyword naming one, or
    // '(' 'ref' ... for a reference to a named heap type.
    bool is_valtype_start();
    // Parses a functype rule '(' 'func' param* result* ')'
    std::pair<FuncType, std::map<std::string, index_t>> parse_functype();
    // Parses one '(' 'type' id? subtype ')' and returns its index.
    index_t parse_typedef();
    // subtype ::= '(' 'sub' 'final'? typeidx* comptype ')' | comptype
    std::pair<SubType, std::map<std::string, index_t>> parse_subtype(index_t self);
    // comptype ::= functype | '(' 'struct' field* ')' | '(' 'array' fieldtype ')'
    std::pair<CompType, std::map<std::string, index_t>> parse_comptype();
    FieldType   parse_fieldtype();   // storagetype | '(' 'mut' storagetype ')'
    StorageType parse_storagetype(); // valtype | i8 | i16
    void resolve_pending_supers();

    Limits     parse_limits();
    MemType    parse_memtype();
    TableType  parse_tabletype();
    GlobalType parse_globaltype();

    // Returns true if the next tokens could start a typeuse clause.
    bool is_typeuse_start();
    // Parses typeuse, always returns an index (creates empty type if needed).
    index_t parse_typeuse();

    // Parses a param clause; returns either (id, type, loc) or list of types.
    // Returned as pair<string, vector<ValueType>>: id may be empty.
    std::pair<std::string, std::vector<ValueType>> parse_param();
    std::vector<ValueType> parse_result();
    std::pair<std::string, std::vector<ValueType>> parse_local();

    // ── Import / export helpers ──────────────────────────────────────────────
    std::pair<std::string, std::string> parse_importabbr();  // (module, name)
    std::string parse_exportabbr();
    std::pair<WasmExport::DescType, index_t> parse_exportdesc();

    // ── Index parsers ────────────────────────────────────────────────────────
    index_t parse_typeidx();
    // A field of struct type `typeidx`, by id or by number.
    index_t parse_fieldidx(index_t typeidx);
    index_t parse_funcidx();
    index_t parse_tableidx();
    index_t parse_memidx();
    index_t parse_globalidx();
    index_t parse_elemidx();
    index_t parse_dataidx();
    index_t parse_localidx();
    index_t parse_labelidx();
    index_t parse_tagidx();

    // ── Numeric value parsers ────────────────────────────────────────────────
    i32_t parse_i32();
    i64_t parse_i64();
    u32_t parse_u32();
    u64_t parse_u64();
    Token expect_float_literal();
    f32_t parse_f32();
    f64_t parse_f64();
    // Converts a raw quoted string token to bytes (handles all escape sequences).
    std::string parse_string_to_bytes(const std::string& raw);

    // ── Elem / constexpr helpers ─────────────────────────────────────────────
    ConstInstr parse_constexpr();
    ConstInstr parse_elemexpr();
    std::pair<ValueType, std::vector<ConstInstr>> parse_elemlist();

    // ── Label ────────────────────────────────────────────────────────────────
    std::string parse_label();   // optionally consumes an Id, returns it or ""

    // ── Instruction parsers (implemented in instr_parse.cpp) ─────────────────
    std::vector<WasmInstr> parse_instr();
    std::vector<WasmInstr> parse_blockinstr(const std::string& kw);
    std::vector<WasmInstr> parse_foldedinstr();
    WasmInstr parse_plaininstr();
    WasmInstr parse_controlinstr(const std::string& name);
    WasmInstr parse_referenceinstr(const std::string& name);
    WasmInstr parse_parametricinstr(const std::string& name);
    WasmInstr parse_variableinstr(const std::string& name);
    WasmInstr parse_tableinstr(const std::string& name);
    WasmInstr parse_memoryinstr(const std::string& name);
    WasmInstr parse_numericinstr(const std::string& name);
    WasmInstr parse_gcinstr(const std::string& name);
    // Parses (ref null? heaptype) and returns {nullable, heaptype-as-int32}.
    std::pair<bool, int32_t> parse_reftype_annot();
    // heaptype ::= abstract keyword | typeidx
    int32_t parse_heaptype();
    std::pair<offset_t, std::optional<align_t>> parse_memarg();

    // ── Post-processing (same logic as original post_process.cpp) ────────────
    void post_process();
    std::unordered_map<index_t, index_t> reorder_map(IndexSpace& space);
};

} // namespace Parse
} // namespace WasmVM

#endif
