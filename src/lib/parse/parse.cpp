// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::istream& stream){
    return WasmModule();
}


// using namespace Exception;
// string_not_close::string_not_close(Token::Location location) : 
//     Parse("string not close", location) {}
// block_comment_not_close::block_comment_not_close(Token::Location location) : 
//     Parse("block comment not close", location) {}
// unknown_token::unknown_token(Token::Location location, std::string token) : 
//     Parse(std::string("unknown token '") + token + "'", location) {}
// unknown_identifier::unknown_identifier(Token::Location location, std::string message) : 
//     Parse(std::string("unknown identifier") + message, location) {}
// invalid_functype::invalid_functype(Token::Location location, std::string message) : 
//     Parse(std::string("invalid functype") + message, location) {}
// invalid_limit::invalid_limit(Token::Location location, std::string message) : 
//     Parse(std::string("invalid limit") + message, location) {}
// duplicated_identifier::duplicated_identifier(Token::Location location, std::string message) :
//     Parse(std::string("duplicated identifier") + message, location) {}
// block_id_mismatch::block_id_mismatch(Token::Location location, std::string message) :
//     Parse(std::string("block id mismatch") + message, location) {}
// index_out_of_range::index_out_of_range(Token::Location location, std::string message) :
//     Parse(std::string("index out of range") + message, location) {}
// invalid_folded_instruction::invalid_folded_instruction(Token::Location location) : 
//     Parse("invalid folded instruction", location) {}
// invalid_immediate_value::invalid_immediate_value(Token::Location location, std::string message) :
//     Parse(std::string("invalid immediate value") + message, location) {}
// multiple_start::multiple_start(Token::Location location) :
//     Parse(std::string("multiple start section"), location) {}