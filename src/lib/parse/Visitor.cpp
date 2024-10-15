// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Visitor.hpp"
#include <WasmVM.hpp>

using namespace WasmVM;

std::any Visitor::visitModule(WatParser::ModuleContext *context){
    visitChildren(context);
    return module;
}
