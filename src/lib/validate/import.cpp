// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"
#include <exception.hpp>

#include <variant>
#include "Util.hpp"

using namespace WasmVM;

template<> void Validate::Validator::operator()<WasmImport>(const WasmImport& import){
    std::visit(overloaded {
        [&](const TableType& table){
            operator()(table);
        },
        [&](const MemType& mem){
            operator()(mem);
        },
        [&](auto){},
    }, import.desc);
}