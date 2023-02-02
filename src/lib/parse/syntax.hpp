#ifndef WASMVM_PP_PARSE_SYNTAX_DEF
#define WASMVM_PP_PARSE_SYNTAX_DEF

#include "nodes.hpp"
#include <structures/WasmModule.hpp>

namespace WasmVM {
namespace Syntax {

using TypeUse = Parse::Rule<
    Parse::Optional<
        Parse::Rule<Token::ParenL, Token::Keyword<"type">, Parse::OneOf<Token::Number, Token::Id>, Token::ParenR>
    >,
    Parse::Repeat<
        Parse::Rule<Token::ParenL, Token::Keyword<"param">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::ValueType>, Token::ParenR>
    >,
    Parse::Repeat<
        Parse::Rule<Token::ParenL, Token::Keyword<"result">, Parse::Repeat<Parse::ValueType>, Token::ParenR>
    >
>;

using Limits = Parse::Rule<
    Token::Number, Parse::Optional<Token::Number>
>;

using RefType = Parse::OneOf<
    Token::Keyword<"funcref">, Token::Keyword<"externref">
>;

namespace ImportDesc {
using Func = Parse::Rule<Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, Syntax::TypeUse, Token::ParenR>;
using Table = Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Optional<Token::Id>, Parse::TableType, Token::ParenR>;
using Memory = Parse::Rule<Token::ParenL, Token::Keyword<"memory">, Parse::Optional<Token::Id>, Parse::MemType, Token::ParenR>;
}

}
}

#endif