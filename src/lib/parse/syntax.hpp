#ifndef WASMVM_PP_PARSE_SYNTAX_DEF
#define WASMVM_PP_PARSE_SYNTAX_DEF

#include "nodes.hpp"
#include <structures/WasmModule.hpp>

namespace WasmVM {
namespace Syntax {

using Limits = Parse::Rule<
    Token::Number, Parse::Optional<Token::Number>
>;

using RefType = Parse::OneOf<
    Token::Keyword<"funcref">, Token::Keyword<"externref">
>;

using Local = Parse::Rule<
    Token::ParenL, Token::Keyword<"local">, Parse::Optional<Token::Id>, Parse::Repeat<Parse::ValueType>, Token::ParenR
>;

namespace ImportDesc {
using Func = Parse::Rule<Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, Parse::TypeUse, Token::ParenR>;
using Table = Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Optional<Token::Id>, Parse::TableType, Token::ParenR>;
using Memory = Parse::Rule<Token::ParenL, Token::Keyword<"memory">, Parse::Optional<Token::Id>, Parse::MemType, Token::ParenR>;
using Global = Parse::Rule<Token::ParenL, Token::Keyword<"global">, Parse::Optional<Token::Id>, Parse::GlobalType, Token::ParenR>;
}

using PlainInstr = Parse::OneOf<
    Parse::Instr::Unreachable,
    Parse::Instr::Nop,
    Parse::Instr::Call
>;

using Instr = Parse::OneOf<Syntax::PlainInstr, Parse::Instr::Block, Parse::Instr::Loop>;

}
}

#endif