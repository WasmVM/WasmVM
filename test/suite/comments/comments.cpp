// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

#include <fstream>
#include <sstream>
#include <list>

using namespace WasmVM;
using namespace Testing;

Suite comments {
    Test("Line comments", {
        std::ifstream stream("line.wat");
        Expect(tokenize(stream).size() == 3);
        stream.close();
    })

    Category("Block comments", {
        Test("Regular", {
            std::ifstream stream("block.wat");
            Expect(tokenize(stream).size() == 3);
            stream.close();
        })
        Test("Not close", {
            std::ifstream stream("not_close.wat");
            Throw(WasmVM::Exception::block_comment_not_close, 
                Expect(tokenize(stream).size() == 3);
            )
            stream.close();
        })
    })

    Test("Nested comments", {
        std::ifstream stream("nested.wat");
        Expect(tokenize(stream).size() == 6);
        stream.close();
    })

    Test("Whitespace", {
        std::stringstream stream("  \n \t");
        Expect(tokenize(stream).size() == 0);
    })
};