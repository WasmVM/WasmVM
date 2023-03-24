#ifndef WASMVM_PP_PARSE_SYNTAX_DEF
#define WASMVM_PP_PARSE_SYNTAX_DEF

#include <list>
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

namespace ImportDesc {
using Func = Parse::Rule<Token::ParenL, Token::Keyword<"func">, Parse::Optional<Token::Id>, Parse::TypeUse, Token::ParenR>;
using Table = Parse::Rule<Token::ParenL, Token::Keyword<"table">, Parse::Optional<Token::Id>, Parse::TableType, Token::ParenR>;
using Memory = Parse::Rule<Token::ParenL, Token::Keyword<"memory">, Parse::Optional<Token::Id>, Parse::MemType, Token::ParenR>;
using Global = Parse::Rule<Token::ParenL, Token::Keyword<"global">, Parse::Optional<Token::Id>, Parse::GlobalType, Token::ParenR>;
}

using PlainInstr = Parse::OneOf<
    Parse::Instr::Unreachable,
    Parse::Instr::Nop,
    Parse::Instr::Br,
    Parse::Instr::Br_if,
    Parse::Instr::Br_table,
    Parse::Instr::Return,
    Parse::Instr::Call,
    Parse::Instr::Call_indirect,

    Parse::Instr::Ref_null,
    Parse::Instr::Ref_is_null,
    Parse::Instr::Ref_func,

    Parse::Instr::Drop,
    Parse::Instr::Select,

    Parse::Instr::Local_get,
    Parse::Instr::Local_set,
    Parse::Instr::Local_tee,
    Parse::Instr::Global_get,
    Parse::Instr::Global_set,

    Parse::Instr::Table_get,
    Parse::Instr::Table_set,
    Parse::Instr::Table_size,
    Parse::Instr::Table_grow,
    Parse::Instr::Table_fill,
    Parse::Instr::Table_copy,
    Parse::Instr::Table_init,
    Parse::Instr::Elem_drop,

    Parse::Instr::I32_load,
    Parse::Instr::I64_load,
    Parse::Instr::F32_load,
    Parse::Instr::F64_load,
    Parse::Instr::I32_load8_s,
    Parse::Instr::I32_load8_u,
    Parse::Instr::I32_load16_s,
    Parse::Instr::I32_load16_u,
    Parse::Instr::I64_load8_s,
    Parse::Instr::I64_load8_u,
    Parse::Instr::I64_load16_s,
    Parse::Instr::I64_load16_u,
    Parse::Instr::I64_load32_s,
    Parse::Instr::I64_load32_u,
    Parse::Instr::I32_store,
    Parse::Instr::I64_store,
    Parse::Instr::F32_store,
    Parse::Instr::F64_store,
    Parse::Instr::I32_store8,
    Parse::Instr::I32_store16,
    Parse::Instr::I64_store8,
    Parse::Instr::I64_store16,
    Parse::Instr::I64_store32,
    Parse::Instr::Memory_size, 
    Parse::Instr::Memory_grow,
    Parse::Instr::Memory_fill,
    Parse::Instr::Memory_copy,
    Parse::Instr::Memory_init,
    Parse::Instr::Data_drop,
    Parse::Instr::I32_const,
    Parse::Instr::I64_const,
    Parse::Instr::F32_const,
    Parse::Instr::F64_const,
    Parse::Instr::NumericInstr
>;

using Instr = Parse::OneOf<
    Syntax::PlainInstr,
    Parse::Instr::Block,
    Parse::Instr::Loop,
    Parse::Instr::If
>;

using ConstInstr = Parse::OneOf<
    Parse::Instr::Ref_null,
    Parse::Instr::Ref_func,
    Parse::Instr::Global_get,
    Parse::Instr::I32_const,
    Parse::Instr::I64_const,
    Parse::Instr::F32_const,
    Parse::Instr::F64_const
>;

struct FoldedInstr {
    static std::optional<FoldedInstr> get(TokenIter& begin, TokenHolder& holder);
    std::list<Syntax::Instr> instrs;
};

}
}

#endif