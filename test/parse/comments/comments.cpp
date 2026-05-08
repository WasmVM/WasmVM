// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/Tokenizer.hpp>
#include <exception.hpp>

#include <fstream>
#include <sstream>

using namespace WasmVM;
using namespace WasmVM::Parse;
using namespace Testing;

Suite comments {
    Test("Line comments", {
        std::ifstream fin("line.wat");
        Tokenizer tok(fin);
        // Three (module) blocks separated by line comments
        for (int i = 0; i < 3; ++i) {
            Expect(tok.peek().type == TokenType::LParen);  tok.consume();
            Expect(tok.peek_keyword("module"));            tok.consume();
            Expect(tok.peek().type == TokenType::RParen);  tok.consume();
        }
        Expect(tok.at_eof());
    })

    Category("Block comments", {
        Test("Regular", {
            // Open in binary so MSVC text-mode doesn't truncate at the
            // 0x1A (Ctrl+Z) byte that this fixture contains intentionally.
            std::ifstream fin("block.wat", std::ios::binary);
            Tokenizer tok(fin);
            Expect(tok.peek().type == TokenType::LParen);  tok.consume();
            Expect(tok.peek_keyword("module"));            tok.consume();
            Expect(tok.peek().type == TokenType::RParen);  tok.consume();
            Expect(tok.at_eof());
        })
        Test("Not close", {
            std::ifstream fin("not_close.wat");
            bool exceptionThrown = false;
            try {
                Tokenizer tok(fin);
                while (!tok.at_eof()) { tok.consume(); }
            } catch (const WasmVM::Exception::Parse&) {
                exceptionThrown = true;
            }
            Expect(exceptionThrown);
        })
    })

    Test("Nested comments", {
        std::ifstream fin("nested.wat");
        Tokenizer tok(fin);
        // Two (module) blocks with nested block comments inside/around them
        for (int i = 0; i < 2; ++i) {
            Expect(tok.peek().type == TokenType::LParen);  tok.consume();
            Expect(tok.peek_keyword("module"));            tok.consume();
            Expect(tok.peek().type == TokenType::RParen);  tok.consume();
        }
        Expect(tok.at_eof());
    })

    Test("Whitespace", {
        std::istringstream input("  \n \t");
        Tokenizer tok(input);
        Expect(tok.at_eof());
    })
};
