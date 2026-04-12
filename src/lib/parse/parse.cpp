// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>
#include "Tokenizer.hpp"
#include "ParseContext.hpp"

using namespace WasmVM;

WasmModule WasmVM::module_parse(std::istream& is) {
    Parse::Tokenizer tokenizer(is);
    Parse::ParseContext ctx(tokenizer);
    return ctx.parse();
}
