// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmFunc& func){
    return dump_func(stream, func, std::nullopt);
}

// `funcidx` is the function's index, which names it, its locals and its
// labels; without it only the references in the body are named.
std::ostream& WasmVM::dump_func(std::ostream& stream, const WasmFunc& func, std::optional<index_t> funcidx){
    DumpNames* names = funcidx ? DumpNames::current() : nullptr;
    stream << "  (func";
    if(names){
        // Defined before its body: a function may call itself.
        dump_def_name(stream, names->define(DumpSpace::func, funcidx.value()));
        names->begin_func(funcidx.value());
    }
    stream << " (type ";
    dump_ref(stream, DumpSpace::type, func.typeidx) << ")";
    index_t params = 0;
    if(names){
        // Locals are indexed after the parameters.
        params = names->param_count(funcidx.value());
    }
    for(index_t i = 0; i < func.locals.size(); ++i){
        stream << " (local";
        if(names){
            dump_def_name(stream, names->local(params + i));
        }
        stream << " " << func.locals[i] << ")";
    }
    stream << std::endl;
    for(auto it = func.body.begin(); it != (func.body.end() - 1); it = std::next(it)){
        stream << "    " << *it << std::endl;
    }
    if(names){
        names->end_func();
    }
    stream << "  )" << std::endl;
    return stream;
}