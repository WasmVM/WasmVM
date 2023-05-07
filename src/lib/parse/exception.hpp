#ifndef WASMVM_PP_PARSE_EXCEPTION_DEF
#define WASMVM_PP_PARSE_EXCEPTION_DEF

#include <exception.hpp>
#include "Token.hpp"

namespace WasmVM {
namespace Exception {
    struct string_not_close : public Parse {
        string_not_close(Token::Location location);
    };
    struct block_comment_not_close : public Parse {
        block_comment_not_close(Token::Location location);
    };
    struct unknown_token : public Parse {
        unknown_token(Token::Location location, std::string token);
    };
    struct unknown_identifier : public Parse {
        unknown_identifier(Token::Location location, std::string message);
    };
    struct invalid_functype : public Parse {
        invalid_functype(Token::Location location, std::string message);
    };
    struct invalid_limit : public Parse {
        invalid_limit(Token::Location location, std::string message);
    };
    struct duplicated_identifier : public Parse {
        duplicated_identifier(Token::Location location, std::string message);
    };
    struct index_out_of_range : public Parse {
        index_out_of_range(Token::Location location, std::string message);
    };
    struct block_id_mismatch : public Parse {
        block_id_mismatch(Token::Location location, std::string message);
    };
    struct invalid_folded_instruction : public Parse {
        invalid_folded_instruction(Token::Location location);
    };
    struct invalid_immediate_value : public Parse {
        invalid_immediate_value(Token::Location location, std::string message);
    };
    struct multiple_start : public Parse {
        multiple_start(Token::Location location);
    };
}
}

#endif