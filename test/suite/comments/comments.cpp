// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>

using namespace Testing;

Suite comments {
    Test("Line comments", {
        // Parse(module_line,
        //     ";; Test comment syntax

        //     ;;comment

        //     ;;;;;;;;;;;

        //         ;;comment

        //     ( ;;comment
        //     module ;;comment
        //     );;comment"
        // );
    })

    Category("Blocked", {
        Test("Block comments", {
            Expect(false)
            Assert(true)
            Expect(false)
            // Load(module_block, "block.wat");
        })

        Test("Nested comments", {
            Expect(true)
            Assert(true)
            // Load(module_block, "nested.wat");
        })
    })
};