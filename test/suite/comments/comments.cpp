// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

#include <list>

using namespace WasmVM;
using namespace Testing;

Suite comments {
    Test("Line comments", 
        Expect(tokenize(read_text("line.wat")).size() == 3);
    )

    Category("Block comments", {
        Test("Regular", {
            Expect(tokenize(read_text("block.wat")).size() == 3);
        })
        Test("Not close", {
            Throw(WasmVM::Exception::block_comment_not_close, 
                tokenize(read_text("not_close.wat"));
            )
        })
    })

    Test("Nested comments", {
        Expect(tokenize(read_text("nested.wat")).size() == 6);
    })

    Test("Whitespace", {
        Expect(tokenize("  \n \t").size() == 0);
    })
};