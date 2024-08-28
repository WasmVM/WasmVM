// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

using namespace WasmVM;
using namespace Testing;

Suite func {
    Test("with_index", {
        ParseFile(test_module, "with_index.wat");
        std::optional<index_t>& start = test_module.start;
        Expect(start.has_value());
        Expect(start.value() == 0);
    })
    Test("with_id", {
        ParseFile(test_module, "with_id.wat");
        std::optional<index_t>& start = test_module.start;
        Expect(start.has_value());
        Expect(start.value() == 0);
    })
};