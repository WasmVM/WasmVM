// Copyright 2022 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <WasmVM.hpp>

#include <sstream>

namespace WasmVM {

WasmModule module_parse(std::string text){
    std::istringstream stream(text);
    return module_parse(stream);
}
WasmModule module_parse(std::istream& stream){
    return WasmModule();
}

}