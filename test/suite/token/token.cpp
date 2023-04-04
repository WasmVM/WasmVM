// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

#include <list>

using namespace WasmVM;
using namespace Testing;

Suite token {
    Category("parentheses", {
        Test("valid", {
            ParseFile(module_paren, "paren.wat");
        })
        Category("malformed", {
            Test("unmatched", {
                Throw(WasmVM::Exception::Parse, 
                    module_parse("(()")
                )
            })
            Test("open pair", {
                // Throw(WasmVM::Exception::Parse, 
                //     module_parse(")(")
                // )
            })
        })
    })
};