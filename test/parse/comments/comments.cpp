// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/WatLexer.h>
#include <parse/ErrorListener.hpp>

#include <fstream>

using namespace WasmVM;
using namespace Testing;

Suite comments {
    Test("Line comments", {
        std::ifstream fin("line.wat");
        antlr4::ANTLRInputStream input(fin);
        WasmVM::WatLexer lexer(&input);
        antlr4::CommonTokenStream stream(&lexer);
        stream.fill();
        std::vector<antlr4::Token*> tokens = stream.getTokens();
        Expect(tokens[0]->getText() == "(");
        Expect(tokens[1]->getText() == "module");
        Expect(tokens[2]->getText() == ")");
        Expect(tokens[3]->getText() == "(");
        Expect(tokens[4]->getText() == "module");
        Expect(tokens[5]->getText() == ")");
        Expect(tokens[6]->getText() == "(");
        Expect(tokens[7]->getText() == "module");
        Expect(tokens[8]->getText() == ")");
        Expect(tokens[9]->getText() == "<EOF>");
    })

    Category("Block comments", {
        Test("Regular", {
            std::ifstream fin("block.wat");
            antlr4::ANTLRInputStream input(fin);
            WasmVM::WatLexer lexer(&input);
            antlr4::CommonTokenStream stream(&lexer);
            stream.fill();
            std::vector<antlr4::Token*> tokens = stream.getTokens();
            Expect(tokens[0]->getText() == "(");
            Expect(tokens[1]->getText() == "module");
            Expect(tokens[2]->getText() == ")");
            Expect(tokens[3]->getText() == "<EOF>");
        })
        Test("Not close", {
            std::ifstream fin("not_close.wat");
            antlr4::ANTLRInputStream input(fin);
            WasmVM::WatLexer lexer(&input);
            WasmVM::LexerErrorListener listener;
            lexer.removeErrorListeners();
            lexer.addErrorListener(&listener);
            antlr4::CommonTokenStream stream(&lexer);
            bool exceptionThrown = false;
            try {
                stream.fill();
            } catch (const WasmVM::Exception::Parse&) {
                exceptionThrown = true;
            }
            Expect(exceptionThrown);
        })
    })

    Test("Nested comments", {
        std::ifstream fin("nested.wat");
        antlr4::ANTLRInputStream input(fin);
        WasmVM::WatLexer lexer(&input);
        antlr4::CommonTokenStream stream(&lexer);
        stream.fill();
        std::vector<antlr4::Token*> tokens = stream.getTokens();
        Expect(tokens[0]->getText() == "(");
        Expect(tokens[1]->getText() == "module");
        Expect(tokens[2]->getText() == ")");
        Expect(tokens[3]->getText() == "(");
        Expect(tokens[4]->getText() == "module");
        Expect(tokens[5]->getText() == ")");
        Expect(tokens[6]->getText() == "<EOF>");
    })

    Test("Whitespace", {
        antlr4::ANTLRInputStream input("  \n \t");
        WasmVM::WatLexer lexer(&input);
        antlr4::CommonTokenStream stream(&lexer);
        stream.fill();
        std::vector<antlr4::Token*> tokens = stream.getTokens();
        Expect(tokens[0]->getText() == "<EOF>");
    })
};