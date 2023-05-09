#include "decode.hpp"
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

template<> Stream& Decode::operator>> <WasmData>(Stream& stream, WasmData& data){
    u8_t prefix = (u8_t)stream.get<byte_t>();
    if(prefix > 0x02){
        throw Exception::invalid_data(stream.location());
    }
    if(prefix & 0x2){
        stream >> data.mode.memidx.emplace();
    }
    if(prefix & 0x1){
        data.mode.type = WasmData::DataMode::Mode::passive;
    }else{
        stream >> data.mode.offset.emplace();
        data.mode.type = WasmData::DataMode::Mode::active;
    }
    return stream >> data.init;
}

Stream& Decode::Data::read(Stream& stream){
    auto size = header(stream);
    if(size){
        if(stream.get<u32_t>() != datas.size()){
            throw Exception::datacount_mismatch(stream.location());
        }
        for(WasmData& data : datas){
            stream >> data;
        }
    }
    return stream;
}

Stream& Decode::DataCount::read(Stream& stream){
    auto size = header(stream);
    if(size){
        datas.resize(stream.get<u32_t>());
    }
    return stream;
}