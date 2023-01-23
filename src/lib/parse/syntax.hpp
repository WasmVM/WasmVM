#ifndef WASMVM_PP_PARSE_SYNTAX_DEF
#define WASMVM_PP_PARSE_SYNTAX_DEF

#include "nodes.hpp"
#include <structures/WasmModule.hpp>

namespace WasmVM {
namespace Syntax {

using FuncType = Parse::Rule<
    Token::ParenL, Token::Keyword<"type", true>, Parse::Optional<Token::Id>, Parse::FuncType, Token::ParenR
>;

struct Visitor {
    Visitor(WasmModule& module) : module(module){}
    WasmModule& module;

    void operator()(FuncType& type);
};

}
}

#endif