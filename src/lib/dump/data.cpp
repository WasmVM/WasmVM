// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

#include <iomanip>

using namespace WasmVM;

std::ostream& WasmVM::operator<<(std::ostream& stream, const WasmData& data){
    return dump_data(stream, data, nullptr);
}

std::ostream& WasmVM::dump_data(std::ostream& stream, const WasmData& data, const std::string* name){
    stream << "  (data";
    dump_def_name(stream, name) << " ";
    switch(data.mode.type){
        case WasmData::DataMode::Mode::active :
            stream << "(memory ";
            dump_ref(stream, DumpSpace::mem, data.mode.memidx.value()) << ") ";
            stream << "(offset " << data.mode.offset.value() << ") ";
        break;
        default:
        break;
    }
    stream << std::endl << "    \"";
    auto old_fmt = stream.flags();
    for(const byte_t& item : data.init){
        stream << "\\" << std::hex << std::setfill('0') << std::setw(2) << (int)item;
    }
    stream.flags(old_fmt);
    return stream << "\"" << std::endl << "  )" << std::endl;
}