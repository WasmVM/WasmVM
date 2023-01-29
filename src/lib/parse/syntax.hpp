#ifndef WASMVM_PP_PARSE_SYNTAX_DEF
#define WASMVM_PP_PARSE_SYNTAX_DEF

#include "nodes.hpp"
#include <structures/WasmModule.hpp>

namespace WasmVM {
namespace Syntax {

using FuncType = Parse::Rule<
    Token::ParenL, Token::Keyword<"type">, Parse::Optional<Token::Id>, Parse::FuncType, Token::ParenR
>;

using TypeUse = Parse::Rule<
    Parse::Optional<
        Parse::Rule<Token::ParenL, Token::Keyword<"type">, Token::Number, Token::ParenR>
    >,
    Parse::Repeat<
        Parse::Rule<Token::ParenL, Token::Keyword<"param">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::ValueType>, Token::ParenR>
    >,
    Parse::Repeat<
        Parse::Rule<Token::ParenL, Token::Keyword<"result">, Parse::Repeat<Parse::ValueType>, Token::ParenR>
    >
>;

namespace ImportDesc {

using Func = Parse::Rule<Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, Syntax::TypeUse, Token::ParenR>;

}

}
}

#endif