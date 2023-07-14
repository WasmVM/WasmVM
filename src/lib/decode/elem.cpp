// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <WasmElem>(Stream& stream, WasmElem& elem){
    u8_t prefix = (u8_t)stream.get<byte_t>();
    if(prefix > 0x07){
        throw Exception::invalid_elem(stream.location());
    }
    if(prefix & 0x1){
        if(prefix & 0x4){
            // 0x05, 0x07
            stream >> elem.type >> elem.elemlist;
        }else{
            // 0x01, 0x03
            elem.type = RefType::funcref;
            if(stream.get<byte_t>() != (byte_t)0x00){
                throw Exception::invalid_elem(stream.location());
            }
            std::vector<index_t> funcidxs;
            stream >> funcidxs;
            for(index_t funcidx : funcidxs){
                elem.elemlist.emplace_back<Instr::Ref_func>(funcidx);
            }
        }
        elem.mode.type = (prefix & 0x2) ? WasmElem::ElemMode::Mode::declarative : WasmElem::ElemMode::Mode::passive;
    }else{
        elem.mode.type = WasmElem::ElemMode::Mode::active;
        elem.type = RefType::funcref;
        if(prefix & 0x2){
            stream >> elem.mode.tableidx.emplace() >> elem.mode.offset.emplace();
            if(prefix & 0x4){
                // 0x06
                stream >> elem.type;
            }else if(stream.get<byte_t>() != (byte_t)0x00){
                throw Exception::invalid_elem(stream.location());
            }
        }else{
            elem.mode.tableidx = 0;
            stream >> elem.mode.offset.emplace();
        }
        if(prefix & 0x4){ // 0x04, 0x06
            stream >> elem.elemlist;
        }else{ // 0x00, 0x02
            std::vector<index_t> funcidxs;
            stream >> funcidxs;
            for(index_t funcidx : funcidxs){
                elem.elemlist.emplace_back<Instr::Ref_func>(funcidx);
            }
        }
    }
    return stream;
}

Stream& Decode::Elem::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> elems;
    }
    return stream;
}