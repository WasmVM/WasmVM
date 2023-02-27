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
    "unreachable", "nop", "block", "end", "loop", "if", "else", "br", "br_if", "br_table", "return", "call", "call_indirect",
    "ref.null", "ref.is_null", "ref.func",
    "drop", "select",
    "local.get","local.set","local.tee","global.get","global.set",
};

#endif