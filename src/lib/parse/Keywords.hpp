#ifndef WASMVM_PP_PARSE_KEYWORDS_DEF
#define WASMVM_PP_PARSE_KEYWORDS_DEF

#include <unordered_set>
#include <string>

static std::unordered_set<std::string> keywords = {
    "module", "type", "import",
    "param", "result",
    "funcref", "extern", "externref",
    "i32", "i64", "f32", "f64",
    "func", "table", "memory", "global",
    "mut", "local",
    "unreachable", "nop", "block", "end", "loop", "if", "then", "else", "br", "br_if", "br_table", "return", "call", "call_indirect",
    "ref.null", "ref.is_null", "ref.func",
    "drop","select",
    "local.get","local.set","local.tee","global.get","global.set",
    "table.get","table.set","table.size","table.grow","table.fill","table.copy","table.init","elem.drop",
    "memory.size", "memory.grow", "memory.fill", "memory.init", "memory.copy", "data.drop",
    "i32.load","i64.load","f32.load","f64.load","i32.load8_s","i32.load8_u","i32.load16_s","i32.load16_u",
    "i64.load8_s","i64.load8_u","i64.load16_s","i64.load16_u","i64.load32_s","i64.load32_u",
    "i32.store","i64.store","f32.store","f64.store","i32.store8","i32.store16","i64.store8","i64.store16","i64.store32",
    "i32.const","i64.const","f32.const","f64.const",
};

#endif