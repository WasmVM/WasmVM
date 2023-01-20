#ifndef WASMVM_PP_PARSE_KEYWORDS_DEF
#define WASMVM_PP_PARSE_KEYWORDS_DEF

#include <unordered_set>
#include <string>

static std::unordered_set<std::string> keywords = {
    "module", "type",
    "param", "result",
    "func", "funcref", "extern", "externref",
    "i32", "i64", "f32", "f64",
};

#endif