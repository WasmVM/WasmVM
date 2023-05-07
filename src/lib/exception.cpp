// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <exception.hpp>

using namespace WasmVM;

std::optional<std::function<void(std::string)>> Exception::Warning::handler = std::nullopt;

Exception::unexpected_eof::unexpected_eof() :
    Exception(std::string("unexpected EOF")) {}