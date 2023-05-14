// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "validate.hpp"
#include "state.hpp"

#include <vector>

using namespace WasmVM;

template<> void Validate::Validator::operator()<WasmFunc>(const WasmFunc& func){
}