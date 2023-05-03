// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

using namespace WasmVM;

Encode::Section::Stream::Stream(std::ostream& stream) : stream(stream){}

Encode::Section::Section(const byte_t id) : id(id){}

void Encode::Section::write(std::ostream& ostream){
    if(!buffer.str().empty()){
        Stream(ostream) << id << buffer;
    }
}

std::ostream& WasmVM::module_encode(const WasmModule& module, std::ostream& ostream){
    // Magic & version
    ostream.write(Encode::magic, sizeof(Encode::magic) - 1);
    // Type
    Encode::Type(module.types).write(ostream);
    // Import
    Encode::Import(module.imports).write(ostream);
    // Func
    Encode::Func(module.funcs).write(ostream);
    return ostream;
}