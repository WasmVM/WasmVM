// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/Lexer.hpp>

#include <fstream>
#include <sstream>
#include <list>

using namespace WasmVM;
using namespace Testing;

Suite comments {
    Test("Line comments", {
        std::ifstream stream("line.wat");
        Lexer lexer("line.wat", stream);
        Expect(std::holds_alternative<Tokens::ParenL>(lexer.get()));
        Expect(std::holds_alternative<Tokens::Module>(lexer.get()));
        Expect(std::holds_alternative<Tokens::ParenR>(lexer.get()));
        stream.close();
    })

    Category("Block comments", {
        Test("Regular", {
            std::ifstream stream("block.wat");
            Lexer lexer("block.wat", stream);
            Expect(std::holds_alternative<Tokens::ParenL>(lexer.get()));
            Expect(std::holds_alternative<Tokens::Module>(lexer.get()));
            Expect(std::holds_alternative<Tokens::ParenR>(lexer.get()));
            stream.close();
        })
        Test("Not close", {
            std::ifstream stream("not_close.wat");
            Lexer lexer("not_close.wat", stream);
            Throw(WasmVM::Exception::Parse, 
                lexer.get();
            )
            stream.close();
        })
    })

    Test("Nested comments", {
        std::ifstream stream("nested.wat");
        Lexer lexer("nested.wat", stream);
        Expect(std::holds_alternative<Tokens::ParenL>(lexer.get()));
        Expect(std::holds_alternative<Tokens::Module>(lexer.get()));
        Expect(std::holds_alternative<Tokens::ParenR>(lexer.get()));
        Expect(std::holds_alternative<Tokens::ParenL>(lexer.get()));
        Expect(std::holds_alternative<Tokens::Module>(lexer.get()));
        Expect(std::holds_alternative<Tokens::ParenR>(lexer.get()));
        stream.close();
    })

    Test("Whitespace", {
        std::stringstream stream("  \n \t");
        Lexer lexer("ws.wat", stream);
        Expect(std::holds_alternative<std::monostate>(lexer.get()));
    })
};