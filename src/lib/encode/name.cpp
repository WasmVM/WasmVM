// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "encode.hpp"

using namespace WasmVM;
using namespace Encode;

// The custom "name" section; nothing is written when the module holds no
// names. Subsections are written in increasing id order, empty ones left out.
Encode::Name::Name(const WasmNames& names) : Section((byte_t)0x00){
    if(names.empty()){
        return;
    }
    auto write_namemap = [](Section::Stream& stream, const WasmNames::NameMap& map){
        stream << (u32_t)map.size();
        for(auto& [idx, name] : map){
            stream << idx << name;
        }
    };
    auto write_indirectnamemap = [&](Section::Stream& stream, const WasmNames::IndirectNameMap& map){
        stream << (u32_t)map.size();
        for(auto& [idx, inner] : map){
            stream << idx;
            write_namemap(stream, inner);
        }
    };
    Section::Stream stream(buffer);
    stream << std::string("name");
    auto subsection = [&](u8_t id, auto&& write){
        std::stringstream sub_buffer;
        Section::Stream sub_stream(sub_buffer);
        write(sub_stream);
        stream << (byte_t)id << sub_buffer;
    };
    auto namemap = [&](u8_t id, const WasmNames::NameMap& map){
        if(!map.empty()){
            subsection(id, [&](Section::Stream& s){ write_namemap(s, map); });
        }
    };
    auto indirect = [&](u8_t id, const WasmNames::IndirectNameMap& map){
        if(!map.empty()){
            subsection(id, [&](Section::Stream& s){ write_indirectnamemap(s, map); });
        }
    };
    if(names.module){
        subsection(0, [&](Section::Stream& s){ s << names.module.value(); });
    }
    namemap(1, names.funcs);
    indirect(2, names.locals);
    indirect(3, names.labels);
    namemap(4, names.types);
    namemap(5, names.tables);
    namemap(6, names.mems);
    namemap(7, names.globals);
    namemap(8, names.elems);
    namemap(9, names.datas);
    indirect(10, names.fields);
    namemap(11, names.tags);
    indirect(12, names.type_params);
    indirect(13, names.tag_params);
}
