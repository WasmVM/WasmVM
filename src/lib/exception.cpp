// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <exception.hpp>

using namespace WasmVM;

std::optional<std::function<void(std::string)>> Exception::Warning::handler = std::nullopt;

Exception::unexpected_eof::unexpected_eof() :
    Exception(std::string("unexpected EOF")) {}

Exception::interrupted::interrupted() :
    Exception("interrupted") {}

Exception::Uncaught::Uncaught(GcPtr<WasmException> exn) :
    Exception("unhandled WebAssembly exception"), exn(std::move(exn)) {}

Exception::Exception::~Exception() = default;
Exception::Parse::~Parse() = default;
Exception::Decode::~Decode() = default;
Exception::unexpected_eof::~unexpected_eof() = default;
Exception::interrupted::~interrupted() = default;
Exception::Trap::~Trap() = default;
Exception::Uncaught::~Uncaught() = default;
