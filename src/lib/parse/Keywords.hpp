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
};

#endif