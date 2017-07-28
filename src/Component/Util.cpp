#include <Util.h>

bool Util::bigEndian = true;

void Util::checkEndian(){
    // Check endian
    std::uint32_t i = 0x12345678;
    if(*(char *)&i == 0x12){
        bigEndian = true;
    }else{
        bigEndian = false;
    }
}

std::uint16_t Util::toLittle16(const std::uint16_t &val, bool force){
    if(force || bigEndian){
        return (val >> 8) | (val << 8);
    }else{
        return val;
    }
}
std::uint32_t Util::toLittle32(const std::uint32_t &val, bool force){
    if(force || bigEndian){
        return (std::uint32_t)toLittle16(val >> 16, force) | ((std::uint32_t)toLittle16(val) << 16, force);
    }else{
        return val;
    }
}
std::uint64_t Util::toLittle64(const std::uint64_t &val, bool force){
    if(force || bigEndian){
        return (std::uint64_t)toLittle32(val >> 32, force) | ((std::uint64_t)toLittle32(val) << 32, force);
    }else{
        return val;
    }
}