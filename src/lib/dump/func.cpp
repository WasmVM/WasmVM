// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmFunc& func){
    stream << "  (func (type " << func.typeidx << ")";
    for(ValueType local : func.locals){
        stream << " (local " << local << ")";
    }
    stream << std::endl;
    for(auto it = func.body.begin(); it != (func.body.end() - 1); it = std::next(it)){
        stream << "    " << *it << std::endl;
    }
    stream << "  )" << std::endl;
    return stream;
}