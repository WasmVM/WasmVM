// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <parse/parse.hpp>

using namespace WasmVM;
using namespace Testing;

#define empty_check(SRC) \
    WasmModule testmod = module_parse(SRC); \
    Expect(testmod.types.size() == 0); \
    Expect(testmod.imports.size() == 0); \
    Expect(testmod.funcs.size() == 0); \
    Expect(testmod.tables.size() == 0); \
    Expect(testmod.elems.size() == 0);

Suite module {
    Test("empty", {
        empty_check("()");
    })
    Test("module keyword only", {
        empty_check("(module)");
    })
    Test("EOF", {
        empty_check("");
    })
};