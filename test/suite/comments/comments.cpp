// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

using namespace Testing;

Suite comments {
    Test("Line comments", 
        ParseFile(module_line, "line.wat");
    )

    Category("Block comments", {
        Test("Regular", 
            ParseFile(module_block, "block.wat");
        )
        Test("Not close", 
            Throw(WasmVM::Exception::block_comment_not_close, 
                ParseFile(module_block, "not_close.wat")
            )
        )
    })

    Test("Nested comments", 
        ParseFile(module_block, "nested.wat");
    )
};