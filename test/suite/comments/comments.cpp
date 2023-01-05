// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>

using namespace Testing;

Suite comments {
    Test("Line comments", {
        Parse(module_line, "line.wat");
    })

    Test("Block comments", {
        Parse(module_block, "block.wat");
    })
    
    Test("Nested comments", {
        Parse(module_block, "nested.wat");
    })
};